#include "ctl_ocl.h"
#include <iostream>
#include <CL/cl.hpp>

using namespace CTL;

const char* PROGRAM_NAME = "tutorial6";
const char* KERNEL_NAME = "addCosSquare";
const char* KERNEL_CODE = R"OpenCL_C(

void kernel addCosSquare(global const float* input,
                         global float* output)
{
    const size_t id = get_global_id(0);

    const float cosine = cos((float)id);

    output[id] = input[id] + cosine * cosine;
}

)OpenCL_C";

void tutorial6a(); // pure OpenCL program
void tutorial6b(); // some settings of OpenCLConfig class
void tutorial6c(); // OpenCLConfig-based program
void tutorial6d(); // PinnedMemory
 /* look at CTL */ // ClFileLoader

std::vector<float> getInputData(int nbElements)
{
    std::vector<float> ret(nbElements);
    for(auto i = 0; i < nbElements; ++i)
        ret[i] = std::sin(i) * std::sin(i);
    return ret;
}

void printResult(const std::vector<float>& result)
{
    for(auto val : result)
        std::cout << val << " ";
    std::cout << '\n'
              << std::accumulate(result.begin(), result.end(), 0)
              << std::endl;
}

int main()
{
    // tutorial6a();
    // tutorial6b();
    // tutorial6c();
    tutorial6d();

    return 0;
}

void tutorial6a()
{
    // minimal pure OpenCL example (no live coding)

    // initialize input data
    const auto nbElements = 1000;
    const auto input = getInputData(nbElements);

    // allocate output data
    std::vector<float> output(nbElements);

    try
    {
        // get all platforms (drivers), e.g. NVIDIA, AMD, Intel
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);

        if(all_platforms.empty())
        {
            std::cerr << "No platforms found. Check OpenCL installation!\n";
            std::exit(1);
        }
        cl::Platform default_platform = all_platforms[0];
        std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << '\n';

        // get default device (CPUs, GPUs) of the default platform
        std::vector<cl::Device> all_devices;
        default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
        if(all_devices.empty())
        {
            std::cerr << "No devices found. Check OpenCL installation!\n";
            std::exit(1);
        }

        // create context
        cl::Context context(all_devices);

        // use device[1] because that's a GPU; device[0] is the CPU
        cl::Device default_device = all_devices[0];
        std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << '\n';

        // create buffers (a priori not bound to a device)
        const auto bytes = sizeof(float) * nbElements;
        const cl::Buffer inputBuffer(context, CL_MEM_READ_ONLY, bytes);
        const cl::Buffer outputBuffer(context, CL_MEM_WRITE_ONLY, bytes);

        // Create command queue for first device
        const cl::CommandQueue queue(context, default_device);

        // write input array
        queue.enqueueWriteBuffer(inputBuffer, CL_FALSE, 0, bytes, input.data());

        // Build kernel from source string
        const cl::Program program(context, KERNEL_CODE);
        program.build(all_devices);

        // Create kernel object
        cl::Kernel kernel(program, KERNEL_NAME);

        // Bind kernel arguments to kernel
        kernel.setArg(0, inputBuffer);
        kernel.setArg(1, outputBuffer);

        // enqueue kernel
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange{ nbElements });

        // read result
        queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, bytes, output.data());
    } // OpenCL exception handling
    catch(const cl::Error& err)
    {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")\n";
    }

    // print result
    printResult(output);
}

void tutorial6b()
{
    auto& config = OCL::OpenCLConfig::instance();

    std::cout << std::boolalpha << config.isValid() << '\n';

    auto& context = config.context();

    // default set up devices
    auto printDevices = [&config] {
        std::cout << "device list: ";
        for(const auto& dev : config.devices())
            std::cout << dev.getInfo<CL_DEVICE_NAME>() << ' ';
        std::cout << '\n';
    };
    printDevices();

    // change device list by black/white listing
    config.setDevices({ "GeForce" });
    printDevices();

    config.setDevices({ "" }, { "Intel", "Voodoo3" }, CL_DEVICE_TYPE_GPU);
    printDevices();

    config.setDevices({ "RTX" }, { "Quadro", "Titan"});
    printDevices();

    // add kernels
    config.addKernel(KERNEL_NAME, KERNEL_CODE, PROGRAM_NAME);

    // check if program is ready
    std::cout << config.isReady(PROGRAM_NAME) << '\n';
    //config.prebuild();

    try {
    auto kernel = config.kernel(KERNEL_NAME, PROGRAM_NAME);
    }
    catch (const std::exception& e) {
        std::cerr << "execption thrown: " << e.what();
        std::exit(1);
    }
    std::cout << config.isReady(PROGRAM_NAME) << '\n';
}

void tutorial6c()
{
    auto& config = OCL::OpenCLConfig::instance();

    // add kernel to config "database" -> no compilation happens!
    config.addKernel(KERNEL_NAME, KERNEL_CODE, PROGRAM_NAME);

    // initialize input data
    const auto nbElements = 1000;
    const auto input = getInputData(nbElements);

    // create buffers
    const auto bytes = sizeof (float) * nbElements;
    const cl::Buffer inputBuffer(config.context(), CL_MEM_READ_ONLY, bytes);
    const cl::Buffer outputBuffer(config.context(), CL_MEM_WRITE_ONLY, bytes);

    // create queue
    const cl::CommandQueue queue(config.context(), config.devices().front());

    // write input buffer
    queue.enqueueWriteBuffer(inputBuffer, CL_FALSE, 0, bytes, input.data()); // slow

    // get kernel and compile OCL program
    auto kernel = config.kernel(KERNEL_NAME, PROGRAM_NAME);

    // bind the arguments to the kernel
    kernel->setArg(0, inputBuffer);
    kernel->setArg(1, outputBuffer);

    // enqueue kernel
    queue.enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(nbElements));

    // allocate output
    std::vector<float> output(nbElements);
    queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, bytes, output.data());

    printResult(output);
}

void tutorial6d()
{
    auto& config = OCL::OpenCLConfig::instance();

    // add kernel to config "database" -> no compilation happens!
    config.addKernel(KERNEL_NAME, KERNEL_CODE, PROGRAM_NAME);

    // initialize input data
    const auto nbElements = 1000;
    const auto input = getInputData(nbElements);

    // create queue
    const cl::CommandQueue queue(config.context(), config.devices().front());

    // create buffers using PinnedMemory -> associated with command queue
    OCL::PinnedBufHostWrite<float> inputBuffer(nbElements, queue);
    OCL::PinnedBufHostRead<float> outputBuffer(nbElements, queue);

    // write input array
    inputBuffer.writeToDev(input.data(), false);

    // get kernel and compile OCL program
    auto kernel = config.kernel(KERNEL_NAME, PROGRAM_NAME);

    // bind the arguments to the kernel
    kernel->setArg(0, inputBuffer.devBuffer());
    kernel->setArg(1, outputBuffer.devBuffer());

    // enqueue kernel
    queue.enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(nbElements));

    // read result
    // allocate output result
    std::vector<float> output(nbElements);

    // #1: simple (blocking)
    //outputBuffer.readFromDev(output.data());
    //printResult(output);

    // #2: use pinned memory directly (omit 1 copy)
    //outputBuffer.transferDevToPinnedMem();
    //auto pinnedMemPtr = outputBuffer.hostPtr();
    //for(auto i = 0; i < nbElements; ++i)
    //    std::cout << pinnedMemPtr[i] << " ";

    // #3: non-blocking read
//    auto future = outputBuffer.readFromDevAsync(output.data());
//    // do something else...
//    future.wait();
//    printResult(output);

    // #4: non-blocking read (fully manually)
    cl::Event e;
    outputBuffer.transferDevToPinnedMem(false, &e);
    // do something else....
    e.wait();
    outputBuffer.readFromPinnedMem(output.data());
    printResult(output);
}




