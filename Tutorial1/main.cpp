#include <QApplication>

#include "ctl.h"
#include "ctl_ocl.h"
#include "ctl_qtgui.h"
using namespace CTL;

void tutorial1a();
void tutorial1b();
void tutorial1c();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    tutorial1a();
    tutorial1b();
    tutorial1c();

    return a.exec();
}

void tutorial1a()
{
    io::BaseTypeIO<io::NrrdFileIO> io;

    // specifying our volume data
    auto volume = VoxelVolume<float>::cube(100, 0.5f, 1.0f);
    //io::BaseTypeIO<io::DenFileIO> denIo;
    //auto volume = denIo.readVolume<float>("I:/CTL_tutorials/data/phantom.den");
    //volume.setVoxelSize(0.4f);

    gui::plot(volume);

    // create an AcquisitionSetup with a generic C-arm CT
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(DetectorBinning::Binning4x4));
    setup.setNbViews(10);

    // assign a trajectory
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(500.0));

    // create the projector
    //auto projector = makeProjector<RayCasterProjectorCPU>();
    auto projector = makeProjector<OCL::RayCasterProjector>();

    // configure the projector for our aquisition
    projector->configure(setup);

    // now, we can create projections
    auto projections = projector->project(volume);
    gui::plot(projections);

    // we save the projections

    qInfo() << io.write(projections, "tutorial1a_proj.nrrd");

    auto pMats = GeometryEncoder::encodeFullGeometry(setup);
    qInfo() << io.write(pMats, "pMats.nrrd");
}

void tutorial1b()
{
    // volume data
    auto volume = VoxelVolume<float>::cylinderZ(50.0f, 50.0f, 0.5f, 0.02f);

    // create an AcquisitionSetup with a generic C-arm CT
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(DetectorBinning::Binning2x2));
    setup.setNbViews(10);

    // assign a trajectory
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(500.0));

    qInfo() << setup.system()->photonsPerPixelMean();
    setup.system()->source()->setFluxModifier(0.01);
    qInfo() << setup.system()->photonsPerPixelMean();
    // create a projector
    auto projector = makeProjector<StandardPipeline>();

    // create projections
    projector->configure(setup);
    auto projections = projector->project(volume);

    projector->enablePoissonNoise(false);
    projector->configure(setup);
    auto projectionsNoNoise = projector->project(volume);

    gui::plot(projections);
    gui::plot(projectionsNoNoise);
}

void tutorial1c()
{
    // volume data
    auto volume = SpectralVolumeData::cylinderZ(50.0f, 50.0f, 0.5f, 1.0f,
                                                database::attenuationModel(database::Composite::Water));

    // create an AcquisitionSetup with a generic C-arm CT
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(DetectorBinning::Binning4x4));
    setup.setNbViews(10);

    // assign a trajectory
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(500.0));

    qInfo() << setup.system()->source()->meanEnergy();

    // create a projector
    auto projector = makeProjector<StandardPipeline>();
    projector->enablePoissonNoise(false);
    projector->enableSpectralEffects(false);

    projector->configure(setup);
    auto projections = projector->project(*volume.muVolume(setup.system()->source()->meanEnergy()));

    projector->enableSpectralEffects(true);
    projector->configure(setup);
    auto projectionsSpectral = projector->project(volume);

    gui::plot(projections);
    gui::plot(projectionsSpectral);
    gui::plot(projections - projectionsSpectral);
}
