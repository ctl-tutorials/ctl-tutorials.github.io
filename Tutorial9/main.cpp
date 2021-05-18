#include <QApplication>

#include "ctl.h"
#include "ctl_ocl.h"
#include "ctl_qtgui.h"

void tutorial9a(); // reconstruction interface, FDK
void tutorial9b(); // iterative reconstruction
void tutorial9c(); // special topics

using namespace CTL;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(MessageHandler::qInstaller);
    MessageHandler::instance().blacklistMessageType(QtDebugMsg);

    try {

        tutorial9a();
        //tutorial9b();
        //tutorial9c();

    }  catch (std::exception& err) {
        qCritical() << err.what();
    }

    return a.exec();
}


// reconstruction interface, FDK
void tutorial9a()
{
    // simulate some data
    const auto phantom = VoxelVolume<float>::cylinderZ(75.0f, 150.0f, 0.5f, 0.02f);
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(DetectorBinning::Binning4x4), 300);
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(700.0f));

    auto projector = makeProjector<OCL::RayCasterProjector>();
    auto projections = projector->configureAndProject(setup, phantom);
    gui::plot(projections);


    //////////////////////////////////////////////////////////////////////////////

    // create a reconstruction
    auto reconstructor = OCL::FDKReconstructor();
    reconstructor.connectNotifierToMessageHandler();

    // value return: (configureAnd)reconstructxxx()
    auto init = VoxelVolume<float>(phantom.dimensions(), phantom.voxelSize());
    // optional: init.fill(1.0f);

    bool ok;
    const auto reco = reconstructor.configureAndReconstruct(setup, projections, init, &ok);
    qInfo() << ok;
    gui::plot(reco);

    // in place reconstruction: (configureAnd)reconstructToxxx()
    auto inplaceRecVol = VoxelVolume<float>::cube(256, 1.0f, 0.0f);
    qInfo() << reconstructor.configureAndReconstructTo(setup, projections, inplaceRecVol);

    gui::plot(inplaceRecVol);

    // applicability
    qInfo() << "FDK is applicable to (a short scan):" << reconstructor.isApplicableTo(setup);

    // switch to a wobble scan
    setup.removeAllPrepareSteps();
    setup.applyPreparationProtocol(protocols::WobbleTrajectory(700.0));

    qInfo() << "FDK is applicable to (a wobble scan):" << reconstructor.isApplicableTo(setup);

    // we can run it anyways (but results can be meaningless)
    gui::plot(reconstructor.configureAndReconstruct(setup, projections, inplaceRecVol, &ok));
    qInfo() << ok;
}


// iterative reconstruction - algebraic reconstruction technique (ART)
void tutorial9b()
{
    // simulate some data
    auto phantom = VoxelVolume<float>::cylinderZ(75.0f, 150.0f, 0.5f, 0.02f);
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(DetectorBinning::Binning4x4), 50);
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(700.0f));

    // reduce problem size (save time)
    setup.system()->replaceDetector(new FlatPanelDetector({ 400, 100 }, { 1.0, 1.0 }));
    phantom = VoxelVolume<float>::cylinderZ(75.0f, 30.0f, 0.5f, 0.02f);
    /*
    */

    auto projector = makeProjector<StandardPipeline>();
    projector->connectNotifierToMessageHandler();
    const auto projections = projector->configureAndProject(setup, phantom);

    auto wobbleSetup = setup;
    wobbleSetup.removeAllPrepareSteps();
    wobbleSetup.applyPreparationProtocol(protocols::WobbleTrajectory(700.0f));
    const auto wobbleProjections = projector->configureAndProject(wobbleSetup, phantom);

    // low-dose setting
    prepare::XrayTubeParam::forMilliampereSeconds(0.1)->prepare(*setup.system());
    const auto lowDoseProjections = projector->configureAndProject(setup, phantom);

    //////////////////////////////////////////////////////////////////////////////

    // create a reconstructor
    auto reconstructor = ARTReconstructor();
    reconstructor.connectNotifierToMessageHandler(true);

    //auto init = VoxelVolume<float>::cube(256, 1.0f, 0.0f);
    auto init = VoxelVolume<float>(256, 256, 50, 1.0f, 1.0f, 1.0f);
    init.fill(0.0f);

    //gui::plot(reconstructor.configureAndReconstruct(setup, projections, init));

    // reconstruct the wobble data
    //gui::plot(reconstructor.configureAndReconstruct(wobbleSetup, wobbleProjections, init));



    //////////////////////////////////////////////////////////////////////////////
    // subsets

    /*
    // changing the boundaries of the transition scheme
    MessageHandler::instance().whitelistClassOrFunction("ART");
    reconstructor.defaultSubsetGenerator().setSubsetBounds(10, 4);
    // default [setup.nbViews(), 1]

    // change the transition period
    reconstructor.defaultSubsetGenerator().setTransitionPeriod(2u);

    // disable the transition scheme / using a fixed number of subsets
    reconstructor.defaultSubsetGenerator().setFixedNumberOfSubsets(1);

    gui::plot(reconstructor.configureAndReconstruct(setup, projections, init));
    */

    //////////////////////////////////////////////////////////////////////////////
    // relaxation

    /*
    // by default: automatic estimation (Power method -> documentation)
    reconstructor.setRelaxationByEstimation(setup, init);
    auto estimate = reconstructor.relaxation();
    qInfo() << estimate;

    // manually setting the relaxation
    reconstructor.setRelaxation(1.0e-3);

    // dangers: too low -> slow convergence
    reconstructor.setRelaxation(0.1 * estimate);
    gui::plot(reconstructor.configureAndReconstruct(setup, projections, init));

    // dangers: too high -> divergence
    reconstructor.setRelaxation(10.0f * estimate);
    gui::plot(reconstructor.configureAndReconstruct(setup, projections, init));

    */
    //////////////////////////////////////////////////////////////////////////////
    // regularization

    gui::plot(lowDoseProjections);

    // reconstruct the data
    auto reco = reconstructor.configureAndReconstruct(setup, lowDoseProjections, init);
    gui::plot(reco);

    reconstructor.setRegularizer(new OCL::TVRegularizer(0.1f));
    reco = reconstructor.configureAndReconstruct(setup, lowDoseProjections, init);
    gui::plot(reco);
}


// special topics
void tutorial9c()
{
    // simulate some data
    const auto phantom = SpectralVolumeData::cylinderZ(50.0f, 30.0f, 0.5f, 1.0f,
                                                       attenuationModel(database::Composite::Tissue_Soft));
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(), 100);
    setup.system()->replaceDetector(new FlatPanelDetector({ 640, 100 }, { 1.0, 1.0 }));
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(750.0f));

    // low kV setting -> increase output to compensate
    prepare::XrayTubeParam::forTubeVoltage(70.0)->prepare(*setup.system());
    prepare::XrayTubeParam::forMilliampereSeconds(10.0f)->prepare(*setup.system());

    auto projector = makeProjector<StandardPipeline>();
    const auto projections = projector->configureAndProject(setup, phantom);

    //////////////////////////////////////////////////////////////////////////////

    auto init = VoxelVolume<float>(256, 256, 50, 1.0f, 1.0f, 1.0f);
    init.fill(0.0f);

    /*
    // reconstruct with FDK
    auto FDK = OCL::FDKReconstructor();
    auto recoFDK = FDK.configureAndReconstruct(setup, projections, init);
    gui::plot(recoFDK);

    //FDK.setProjectionFilter(new OCL::ApodizationFilter(OCL::ApodizationFilter::Cosine, 0.5f));
    //gui::plot(FDK.configureAndReconstruct(setup, projections, init));

    // reconstruct with ART
    auto ART = ARTReconstructor();
    ART.connectNotifierToMessageHandler(true);

    gui::plot(ART.configureAndReconstruct(setup, projections, init));

    // reconstruct using a polychromatic forward model
    auto initSpectral = SpectralVolumeData(init, attenuationModel(database::Composite::Water));
    gui::plot(ART.configureAndReconstruct(setup, projections, initSpectral));

    // initialize the ART with the FDK result and then perform a single iteration of "spectral"
    initSpectral = SpectralVolumeData(recoFDK, attenuationModel(database::Composite::Water));
    ART.setMaxNbIterations(1);
    gui::plot(ART.configureAndReconstruct(setup, projections, initSpectral));
    */

    ////////////////////////////////////////////////////////////////////////////

    // iterative FDK
    auto iterFDK = ARTReconstructor();
    iterFDK.setBackprojector(new OCL::FDKReconstructor);
    iterFDK.defaultSubsetGenerator().setFixedNumberOfSubsets(1);
    iterFDK.defaultSubsetGenerator().setOrder(DefaultSubsetGenerator::Adjacent);

    const auto initSpectral = SpectralVolumeData(init, attenuationModel(database::Composite::Water));

    // straight
    gui::plot(iterFDK.configureAndReconstruct(setup, projections, init));
    gui::plot(iterFDK.configureAndReconstruct(setup, projections, initSpectral));

    // with regularization
    iterFDK.setRegularizer(new OCL::TVRegularizer(5.0f));
    gui::plot(iterFDK.configureAndReconstruct(setup, projections, init));
    gui::plot(iterFDK.configureAndReconstruct(setup, projections, initSpectral));

//    // with apodization in FDK
//    iterFDK.setRegularizationEnabled(false);
//    auto apoFDK = makeReconstructor<OCL::FDKReconstructor>();
//    apoFDK->setProjectionFilter(new OCL::ApodizationFilter(OCL::ApodizationFilter::Cosine, 0.75f));
//    iterFDK.setBackprojector(std::move(apoFDK));

//    gui::plot(iterFDK.configureAndReconstruct(setup, projections, init));
//    gui::plot(iterFDK.configureAndReconstruct(setup, projections, initSpectral));
}
