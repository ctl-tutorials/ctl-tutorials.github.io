#include "customoclvolumefilters.h"

#include <QDebug>

DECLARE_SERIALIZABLE_TYPE(VolumeSegmentationFilter)

VolumeSegmentationFilter::VolumeSegmentationFilter(std::vector<float> thresholds)
    : CTL::OCL::GenericOCLVolumeFilter("F:/projects/ctl-tutorials/tutorialA2B/volumesegementationfilter_flexible.cl")
    , m_thresholds(std::move(thresholds))
{
}

void VolumeSegmentationFilter::filter(CTL::VoxelVolume<float> &volume)
{
    try {

        const auto numThreholds = m_thresholds.size();
        const auto bufferSize = sizeof(float) * numThreholds;

        // create a buffer on the GPU and write in the thresholds
        const auto& context = _queue.getInfo<CL_QUEUE_CONTEXT>();
        auto thresholdBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, bufferSize);
        _queue.enqueueWriteBuffer(thresholdBuffer, CL_FALSE, 0, bufferSize, m_thresholds.data());
        _kernel->setArg(3, thresholdBuffer);
        _kernel->setArg(4, static_cast<uint>(numThreholds));

        // executing the "regular" filter routine
        GenericOCLVolumeFilter::filter(volume);

    }  catch (const cl::Error& err) {
        qCritical() << "OpenCL error:" << err.what() << "(" << err.err() << ")";
    }
}

QVariant VolumeSegmentationFilter::parameter() const
{
    auto parMap = GenericOCLVolumeFilter::parameter().toMap();

    parMap.insert("thresholds", QVariantList(m_thresholds.cbegin(), m_thresholds.cend())); // Note: requires Qt >= 5.14

    return parMap;
}

void VolumeSegmentationFilter::setParameter(const QVariant &parameter)
{
    GenericOCLVolumeFilter::setParameter(parameter);

    const auto parMap = parameter.toMap();

    if(parMap.contains("thresholds"))
    {
        const auto thresholds = parMap.value("thresholds").toList();
        m_thresholds.resize(thresholds.size());
        std::transform(thresholds.cbegin(), thresholds.cend(), m_thresholds.begin(),
                       [] (const QVariant& value) { return value.toFloat(); });
    }
}



VolumeSegmentationFilter::VolumeSegmentationFilter()
    : CTL::OCL::GenericOCLVolumeFilter("F:/projects/ctl-tutorials/tutorialA2B/volumesegementationfilter_flexible.cl")
{
}
