#include <QApplication>
#include <QRandomGenerator>

#include "ctl.h"
#include "ctl_ocl.h"
#include "ctl_qtgui.h"

using namespace CTL;

void tutorial4a();
void tutorial4b();
void tutorial4c();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(MessageHandler::qInstaller);
    MessageHandler::instance().blacklistMessageType(QtDebugMsg);

    //tutorial4a();
    //tutorial4b();
    tutorial4c();

    return a.exec();
}

void tutorial4a()
{
    /* SpectralVolumeData factory methods
       --> All of these require a specification of material density! */
    auto ball = SpectralVolumeData::ball(50.0f, 0.5f, 1.0f,
                                         attenuationModel(database::Composite::Water));
    auto cube = SpectralVolumeData::cube(100, 1.0f, 1.0f,
                                         attenuationModel(database::Composite::Water));
    auto cylinder = SpectralVolumeData::cylinderZ(50.0f, 50.0f, 0.5f, 1.0f,
                                                  attenuationModel(database::Composite::Water));

    // How to use an existing VoxelVolume?
    auto voxelVol = VoxelVolume<float>(100, 100, 100, 1.0, 1.0, 1.0);
    voxelVol.allocateMemory();
    // generate random data in [0.75, 1.25] --> g/cm^3
    std::generate(voxelVol.data().begin(), voxelVol.data().end(),
                  [] () { return static_cast<float>(QRandomGenerator::global()->bounded(0.5) + 0.75); } );
    // gui::plot(voxelVol);

    auto specVol = SpectralVolumeData(voxelVol, database::attenuationModel(database::Composite::Water));
    // gui::plot(specVol);

    // What if 'voxelVol' is in attenuation values (1/mm)?
    // generate random data in [0.01, 0.03] --> 1/mm
    std::generate(voxelVol.data().begin(), voxelVol.data().end(),
                  [] () { return static_cast<float>(QRandomGenerator::global()->bounded(0.02) + 0.01); } );

    specVol = SpectralVolumeData(voxelVol, database::attenuationModel(database::Composite::Water));
    // gui::plot(specVol);

    // This is correct ctor:
    specVol = SpectralVolumeData(voxelVol, database::attenuationModel(database::Composite::Water), 50.0f);
    // gui::plot(specVol);
    // gui::plot(specVol.densityVolume());

    // potentially less error-prone way: using a dedicated factory method
    specVol = SpectralVolumeData::fromMuVolume(voxelVol, database::attenuationModel(database::Composite::Water), 50.0f);
    // gui::plot(specVol);
    // gui::plot(specVol.muVolume(50.0f));

    // also allows for input in Hounsfield units (HU)
    // generate random data in [-1000, 1000] --> HU
    std::generate(voxelVol.data().begin(), voxelVol.data().end(),
                  [] () { return static_cast<float>(QRandomGenerator::global()->bounded(-2000.0) + 1000.0); } );
    specVol = SpectralVolumeData::fromHUVolume(voxelVol, database::attenuationModel(database::Composite::Water), 50.0f);

    gui::plot(specVol);
    gui::plot(specVol.muVolume(50.0f));

    // Of course, we can also use data loaded from files:
    auto loadedData = io::BaseTypeIO<io::NrrdFileIO>().readVolume<float>("../../data/phantom.nrrd");
    specVol = SpectralVolumeData::fromMuVolume(loadedData, database::attenuationModel(database::Composite::Tissue_Equivalent_Plastic),
                                               60.0f);
    gui::plot(specVol);
}

void tutorial4b()
{
    // We create a background volume and two ball volumes.
    auto backgroundVol = VoxelVolume<float>::cube(150, 1.0f, 0.02f);
    auto subVolume1 = VoxelVolume<float>::ball(10.0f, 1.0f, 0.05f);
    auto subVolume2 = VoxelVolume<float>::ball(25.0f, 1.0f, 0.10f);

    // Here, we shift the ball volumes to the side
    subVolume1.setVolumeOffset(0.0f, -20.0f, 0.0f);
    subVolume2.setVolumeOffset(0.0f, 30.0f, 0.0f);

    // Many ways to create a composite of those:

    // #0 - "step-by-step"
    // Create an empty composite volume
    CompositeVolume volume;

    // Now, we add our sub-volumes (i.e. background volume and the two balls)
    volume.addSubVolume(backgroundVol); // volume.addSubVolume(std::move(backgroundVol));
    volume.addSubVolume(subVolume1);    // volume.addSubVolume(std::move(subVolume1));
    volume.addSubVolume(subVolume2);    // volume.addSubVolume(std::move(subVolume2));

    // #1 - ctor with move
    CompositeVolume volume1(std::move(backgroundVol), std::move(subVolume1), std::move(subVolume2));

    // #2 - ctor with in-place construction (of the sub-volumes)
    CompositeVolume volume2(VoxelVolume<float>::cube(150, 1.0f, 0.02f),
                            VoxelVolume<float>::ball(10.0f, 1.0f, 0.05f),
                            VoxelVolume<float>::ball(25.0f, 1.0f, 0.10f));

    volume2.subVolume(1).setVolumeOffset(0.0f, -20.0f, 0.0f);
    volume2.subVolume(2).setVolumeOffset(0.0f, 30.0f, 0.0f);

    // #3 - (abitrary) combinations of any of the above
    CompositeVolume volume3(VoxelVolume<float>::cube(150, 1.0f, 0.02f));
    auto subVolume3 = VoxelVolume<float>::ball(10.0f, 1.0f, 0.05f);
    subVolume3.setVolumeOffset(0.0f, -20.0f, 0.0f);
    volume3.addSubVolume(std::move(subVolume3));
    volume3.addSubVolume(VoxelVolume<float>::ball(25.0f, 1.0f, 0.10f));
    volume3.subVolume(2).setVolumeOffset(0.0f, 30.0f, 0.0f);

    // #3b - combinations with arbitrary voxel sizes (possible with all options #0-#3)
    CompositeVolume volume3b(VoxelVolume<float>::cube(150, 1.0f, 0.02f),    // <-- low resolution (1.0mm)
                             VoxelVolume<float>::ball(10.0f, 0.1f, 0.05f),  // <-- high resolution of 0.1mm
                             VoxelVolume<float>::ball(25.0f, 0.5f, 0.10f)); // <-- medium resolution (0.5mm)

    volume3b.subVolume(1).setVolumeOffset(0.0f, -20.0f, 0.0f);
    volume3b.subVolume(2).setVolumeOffset(0.0f, 30.0f, 0.0f);

    gui::plot(volume3);

    // Sub-volumes can also be SpectralVolumeData:
    CompositeVolume volume4(SpectralVolumeData::ball(10.0f, 1.0f, 1.0f, attenuationModel(database::Composite::Water)),
                            SpectralVolumeData::ball(10.0f, 1.0f, 1.0f, attenuationModel(database::Composite::Bone_Cortical)),
                            SpectralVolumeData::cube(150, 1.0f, database::density(database::Composite::Air),
                                                     attenuationModel(database::Composite::Air)));

    volume4.subVolume(0).setVolumeOffset(0.0f, -30.0f, 0.0f);
    volume4.subVolume(1).setVolumeOffset(0.0f, 30.0f, 0.0f);

    // Mixtures
    CompositeVolume volume5(SpectralVolumeData::ball(10.0f, 1.0f, 1.0f, attenuationModel(database::Composite::Water)),
                            SpectralVolumeData::ball(10.0f, 1.0f, 1.0f, attenuationModel(database::Composite::Bone_Cortical)),
                            VoxelVolume<float>::cube(150, 1.0f, 0.1f * density(database::Composite::Air) *
                                                     attenuationModel(database::Composite::Air)->valueAt(50.0f)));
    // NOTE: the VoxelVolume in volume5 must be in 1/mm ! (--> factor '0.1' for 1/cm -> 1/mm)

    volume5.subVolume(0).setVolumeOffset(0.0f, -30.0f, 0.0f);
    volume5.subVolume(1).setVolumeOffset(0.0f, 30.0f, 0.0f);

    // creating some projections...
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(), 10);
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(750.0f));

    auto projector = makeProjector<StandardPipeline>();
    auto projections = projector->configureAndProject(setup, volume3);

    gui::plot(projections);
}

void tutorial4c()
{
    // Of course, we can also use data loaded from files:
    auto loadedData = io::BaseTypeIO<io::NrrdFileIO>().readVolume<float>("../../data/phantom.nrrd");

    TwoMaterialThresholdVolumeDecomposer decomposer(database::attenuationModel(database::Composite::Tissue_Equivalent_Plastic),
                                                    database::attenuationModel(database::Composite::Bone_Equivalent_Plastic),
                                                    1.5f);
    auto compositeVol = decomposer.decompose(loadedData, 60.0f);

    // clamp negative values to zero (can only happen in first subvolume)
    for(auto& voxel : compositeVol.subVolume(0).data())
        if(voxel < 0.0f) voxel = 0.0f;

    gui::plot(compositeVol);

    // creating some projections...
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(), 10);
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(750.0f));

    auto projector = makeProjector<StandardPipeline>();
    auto projections = projector->configureAndProject(setup, compositeVol);

    gui::plot(projections);

    // create projections from individual subvolumes (ie. single-material)
    auto projectionsBone = projector->configureAndProject(setup, compositeVol.subVolume(1));
    gui::plot(projectionsBone);
}
