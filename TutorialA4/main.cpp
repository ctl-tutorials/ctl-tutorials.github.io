#include <QApplication>

#include "ctl.h"
#include "ctl_ocl.h"
#include "ctl_qtgui.h"

#include "custommodels.h"           // see Tutorial A1
#include "customvolumefilters.h"    // see Tutorial A2
#include "digitizationextension.h"
#include "softtissueextension.h"

// helper functions
void testSerialization(CTL::AbstractProjector& projector);
CTL::VoxelVolume<float> phantom();

// implementations
void tutorialA4_1();
void tutorialA4_2();


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(CTL::MessageHandler::qInstaller);
    CTL::MessageHandler::instance().blacklistMessageType(QtDebugMsg);

    try {

        // showcases
        /*
        auto model = std::make_shared<DiscretizingModel>(0.0, 10.0, 10);
        CTL::gui::plot(model);
        auto model2 = std::make_shared<CTL::IdentityModel>()
                * std::make_shared<CTL::StepFunctionModel>(3.0f, 1.0f, CTL::StepFunctionModel::RightIsZero);
        CTL::gui::plot(model2);
        */

        tutorialA4_1();
        tutorialA4_2();

    }  catch (std::exception& err) {
        qCritical() << err.what();
    }

    return a.exec();
}

void tutorialA4_1()
{
    auto setup = CTL::AcquisitionSetup(CTL::makeCTSystem<CTL::blueprints::GenericCarmCT>
                                       (CTL::DetectorBinning::Binning4x4), 10);
    setup.applyPreparationProtocol(CTL::protocols::ShortScanTrajectory(700.0));

    const auto volume = CTL::VoxelVolume<float>::cube(100, 1.0f, 0.02f);

    /*
    auto projector = CTL::makeProjector<CTL::OCL::RayCasterProjector>();
    CTL::gui::plot(projector->configureAndProject(setup, volume));

    auto extension = std::make_shared<DigitizationExtension>(10.0f, 8);
    extension->use(projector.release());
    CTL::gui::plot(extension->configureAndProject(setup, volume));

    testSerialization(*extension);
    */

    // use the ProjectionPipeline class for a bit more structure
    auto pipeline = CTL::makeProjector<CTL::ProjectionPipeline>(new CTL::OCL::RayCasterProjector());
    CTL::gui::plot(pipeline->configureAndProject(setup, volume));

    pipeline->appendExtension(new DigitizationExtension(10.0f, 8));
    CTL::gui::plot(pipeline->configureAndProject(setup, volume));

    testSerialization(*pipeline);
}


void tutorialA4_2()
{
    auto setup = CTL::AcquisitionSetup(CTL::makeCTSystem<CTL::blueprints::GenericCarmCT>
                                       (CTL::DetectorBinning::Binning4x4), 10);
    setup.applyPreparationProtocol(CTL::protocols::ShortScanTrajectory(700.0));

    const auto volume = phantom();
    CTL::gui::plot(volume);

    auto pipeline = CTL::makeProjector<CTL::ProjectionPipeline>(new CTL::OCL::RayCasterProjector());
    CTL::gui::plot(pipeline->configureAndProject(setup, volume));

    pipeline->appendExtension(new SoftTissueExtension(0.0227f));
    CTL::gui::plot(pipeline->configureAndProject(setup, volume));

    // add any arbitrary other extension...
    pipeline->appendExtension(new DigitizationExtension(10.0f, 4));
    CTL::gui::plot(pipeline->configureAndProject(setup, volume));

    testSerialization(*pipeline);
}


void testSerialization(CTL::AbstractProjector& projector)
{
    auto setup = CTL::AcquisitionSetup(CTL::makeCTSystem<CTL::blueprints::GenericCarmCT>
                                       (CTL::DetectorBinning::Binning4x4), 10);
    setup.applyPreparationProtocol(CTL::protocols::ShortScanTrajectory(700.0));
    const auto volume = CTL::VoxelVolume<float>::cube(100, 1.0f, 0.02f);

    const auto proj1 = projector.configureAndProject(setup, volume);

    CTL::JsonSerializer().serialize(projector, "temp.json");
    auto deserializedProjector = CTL::JsonSerializer().deserializeProjector("temp.json");

    const auto proj2 = deserializedProjector->configureAndProject(setup, volume);

    qInfo() << "Difference:" << CTL::metric::RMSE(proj1.cbegin(), proj1.cend(), proj2.cbegin());
}

CTL::VoxelVolume<float> phantom()
{
    // Note: this uses the RawDataIO of the CTL, which is - at the release date of this tutorial -
    // only present in the 'trunk' branch. Hence, if you are using a CTL version prior to v0.4.2
    // you may use the RawDataIO as implemented in tutorial A3 and adjust the following code by
    // removing the CTL::io namespace from the RawDataIO and adding a third dimension value (256)

    // Phantom data from: https://www5.cs.fau.de/research/data/digital-brain-perfusion-phantom/index.html
    auto volumeBrain = CTL::io::BaseTypeIO<CTL::io::RawDataIO<256, 256, char>>()
            .readVolume<float>("F:/projects/ctl-tutorials/data/DynamicBrainPhantom/brain.raw");
    auto volumeSkull = CTL::io::BaseTypeIO<CTL::io::RawDataIO<256, 256, short>>()
            .readVolume<float>("F:/projects/ctl-tutorials/data/DynamicBrainPhantom/skull.raw");

    auto model = std::shared_ptr<CTL::AbstractDataModel>(new CTL::ConstantModel(0.0));
    model += std::make_shared<CTL::RectFunctionModel>(0.5f, 1.5f, 20.0f);
    model += std::make_shared<CTL::RectFunctionModel>(1.5f, 2.5f, 25.0f);
    model += std::make_shared<CTL::RectFunctionModel>(2.5f, 3.5f, 35.0f);
    model += std::make_shared<CTL::RectFunctionModel>(8.5f, 9.5f, 100.0f);

    ModelApplicationFilter(model).filter(volumeBrain);

    auto volume = volumeBrain + volumeSkull;
    volume.setVoxelSize(1.0f);
    return CTL::assist::huToMu(volume, 50.0f);
}
