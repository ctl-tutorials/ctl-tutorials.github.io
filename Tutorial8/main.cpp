#include <QApplication>

#include "ctl.h"
#include "ctl_ocl.h"
#include "ctl_qtgui.h"

void tutorial8a(); // adjusting the AquisitionSetup by means of preparation protocols
void tutorial8b(); // adjusting the AquisitionSetup through manipulation of views
void tutorial8c(); // creating an AquisitionSetup view-by-view

using namespace CTL;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(MessageHandler::qInstaller);
    MessageHandler::instance().blacklistMessageType(QtDebugMsg);

    try {

        tutorial8a();
        //tutorial8b();
        //tutorial8c();

    }  catch (std::exception& err) {
        qCritical() << err.what();
    }

    return a.exec();
}

// adjusting the AquisitionSetup by means of preparation protocols
void tutorial8a()
{
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(), 10);
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(750.0f));

    // used to prepare views
    setup.prepareView(0);
    qInfo().noquote() << setup.system()->gantry()->info();
    setup.prepareView(1);
    qInfo().noquote() << setup.system()->gantry()->info();

    // or to create projections (internally use prepareView() )
    const auto volume = SpectralVolumeData::ball(50.0f, 0.5f, 1.0,
                                                 attenuationModel(database::Composite::Water));
    auto projector = makeProjector<StandardPipeline>();
    gui::plot(projector->configureAndProject(setup, volume));

    ///////////////////////////////////////////////////////////////

    // we want to modify our setup

    // 1) modulate the mAs of the Xray tube
//    auto mAs = std::vector<double>(setup.nbViews());
//    for(uint v = 0; v < setup.nbViews(); ++v)
//        mAs[v] = 0.001 + 0.001 * double(v);
    const auto mAs = Range<double>::linspace(0.001, 0.011, setup.nbViews(), false);

    // setup.applyPreparationProtocol(protocols::TubeCurrentModulation(mAs));

    // the safe way:
    const auto protocol = protocols::TubeCurrentModulation(mAs);
    if(!protocol.isApplicableTo(setup))
    {
        qWarning() << "Protocol is not applicable";
        return;
    }

    setup.applyPreparationProtocol(protocol);

    gui::plot(projector->configureAndProject(setup, volume));

    // we can inspect a setup based on its JSON file
    JsonSerializer().serialize(setup, "setup.json");

    // another protocol: flying focal spot
    setup.applyPreparationProtocol(protocols::FlyingFocalSpot::twoAlternatingSpots({ 0.0, -50.0, 0.0 },
                                                                                   { 0.0,  50.0, 0.0 }));
    gui::plot(setup);
    gui::plot(projector->configureAndProject(setup, volume));
}

// adjusting the AquisitionSetup through manipulation of views
void tutorial8b()
{
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(), 10);
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(750.0f));

    // we can manipulate the views in the setup
    setup.view(5).addPrepareStep(prepare::XrayTubeParam::forTubeVoltage(50.0));

    // let's inspect the system
//    setup.prepareView(0);
//    qInfo().noquote() << setup.system()->source()->info();
//    setup.prepareView(5);
//    qInfo().noquote() << setup.system()->source()->info();
//    setup.prepareView(0);
//    qInfo().noquote() << setup.system()->source()->info();

    // -> no guarantees in which state you find the system
    // -> prepare everything that you rely on being correct
    for(auto& view : setup.views())
        if(!view.hasPrepareStep(prepare::XrayTubeParam::Type))
            view.addPrepareStep(prepare::XrayTubeParam::forTubeVoltage(100.0));

    // let's inspect the system
    setup.prepareView(0);
    qInfo().noquote() << setup.system()->source()->info();
    setup.prepareView(5);
    qInfo().noquote() << setup.system()->source()->info();
    setup.prepareView(0);
    qInfo().noquote() << setup.system()->source()->info();

    // create projections
    const auto volume = SpectralVolumeData::ball(50.0f, 0.5f, 1.0,
                                                 attenuationModel(database::Composite::Water));
    auto projector = makeProjector<StandardPipeline>();
    gui::plot(projector->configureAndProject(setup, volume));

    // now we want to use the knowledge to create a 'tube voltage modulation'
    // -> there is no preparation protocol for that yet -> later tutorial
    setup.removeAllPrepareSteps();
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(750.0f));

    for(uint v = 0; v < setup.nbViews()/2; ++v)
        setup.view(v).addPrepareStep(prepare::XrayTubeParam::forTubeVoltage(70.0));
    for(uint v = setup.nbViews()/2; v < setup.nbViews(); ++v)
        setup.view(v).addPrepareStep(prepare::XrayTubeParam::forTubeVoltage(120.0));

    gui::plot(projector->configureAndProject(setup, volume));

    // alternative:
    auto prep70 = prepare::XrayTubeParam::forTubeVoltage(70.0);
    auto prep120 = prepare::XrayTubeParam::forTubeVoltage(120.0);

    for(uint v = 0; v < setup.nbViews()/2; ++v)
        setup.view(v).addPrepareStep(prep70);
    for(uint v = setup.nbViews()/2; v < setup.nbViews(); ++v)
        setup.view(v).addPrepareStep(prep120);

    // note that this allows us to manipulate the prepare step through the shared pointer
    prep70->setTubeVoltage(140.0);

    gui::plot(projector->configureAndProject(setup, volume));
}

// creating an AquisitionSetup view-by-view
void tutorial8c()
{
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericTubularCT>());
    setup.system()->replaceDetector(setup.system()->detector()->toGeneric());

    // we manually fill in some views (i.e. 'View' objects)
    {
        AcquisitionSetup::View view;
        view.setTimeStamp(13.37); // used in DynamicProjectorExtension
        view.addPrepareStep(prepare::TubularGantryParam::forRotationAngle(5.0_deg));
        view.addPrepareStep(prepare::TubularGantryParam::forPitchPosition(-20.0));
        view.addPrepareStep(prepare::XrayTubeParam::forTubeVoltage(50.0));
        setup.addView(view);
    }
    {
        AcquisitionSetup::View view;
        view.addPrepareStep(prepare::TubularGantryParam::forRotationAngle(45.0_deg));
        view.addPrepareStep(prepare::TubularGantryParam::forPitchPosition(10.0));
        view.addPrepareStep(prepare::XrayTubeParam::forTubeVoltage(75.0));
        view.addPrepareStep(prepare::XrayTubeParam::forMilliampereSeconds(0.001));
        setup.addView(view);
    }
    {
        auto gantryPrep = std::make_shared<prepare::TubularGantryParam>();
        gantryPrep->setPitchPosition(20.0);
        gantryPrep->setRotationAngle(115.0_deg);
        auto tubePrep = prepare::XrayTubeParam::forTubeVoltage(100.0);
        tubePrep->setMilliampereSeconds(0.2);

        // we can check applicability
        if(!gantryPrep->isApplicableTo(*setup.system()))
            qWarning() << "gantryPrep is not applicable";

        AcquisitionSetup::View view;
        view.addPrepareStep(std::move(gantryPrep));
        view.addPrepareStep(std::move(tubePrep));
        view.addPrepareStep(prepare::GenericDetectorParam::forPixelSize({1.2 , 3.0}));
        setup.addView(view);
    }

    // inspect the setup
    gui::plot(setup);

    // create projections
    const auto volume = SpectralVolumeData::ball(50.0f, 0.5f, 1.0,
                                                 attenuationModel(database::Composite::Water));
    auto projector = makeProjector<StandardPipeline>();
    gui::plot(projector->configureAndProject(setup, volume));
}

