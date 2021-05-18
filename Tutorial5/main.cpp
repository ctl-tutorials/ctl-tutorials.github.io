#include <QApplication>

#include "ctl.h"
#include "ctl_ocl.h"
#include "ctl_qtgui.h"

using namespace CTL;

void tutorial5a(); // CTSystem, SimpleCTSystem
void tutorial5b(); // blueprinted systems and how to customize
void tutorial5c(); // serialization of systems and setups
void tutorial5d(); // deserialization of a setup and test it

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(MessageHandler::qInstaller);
    MessageHandler::instance().blacklistMessageType(QtDebugMsg);

    tutorial5a();
    //tutorial5b();
    //tutorial5c();
    //tutorial5d();

    return a.exec();
}

void tutorial5a()
{
    CTSystem system("Tutorial system");

    // add components to our system
    // three main types 1) detector, 2) gantry, 3) source  [4) beam modifiers -> later tutorial]
    system.addComponent(new FlatPanelDetector(QSize(500, 400), QSizeF(1.0, 1.0)));
    system.addComponent(new CarmGantry);
    system.addComponent(new XrayTube);

    // other options...
    system.clear();
    system << new FlatPanelDetector(QSize(500, 400), QSizeF(1.0, 1.0))
           << new CarmGantry
           << makeComponent<XrayTube>();

    // components ctor: all required arguments are fixed parameters of that component!

    // inspecting the system
    qInfo().noquote() << system.overview(); // brief overview
    // qInfo().noquote() << system.isValid();
    // qInfo().noquote() << system.isSimple();
    qInfo().noquote() << system.info();     // full information on all components

        /* similar to:
        for(const auto& comp : system.components())
           qInfo().noquote() << comp->info();
        */


    // All CTL simulations take SimpleCTSystem; what's the difference?
    // --> SimpleCTSystem contains exactly one of 1) - 3)

    // Creation #1 - from a CTSystem
    bool ok;
    SimpleCTSystem simpleSystem = SimpleCTSystem::fromCTSystem(system, &ok);
    qInfo() << "Could convert:" << ok;
    qInfo().noquote() << simpleSystem.overview();
    //qInfo().noquote() << simpleSystem.info();


    // cannot convert is 'system' is not simple
    qWarning() << "We add another source to our system.";
    system.addComponent(new XrayTube);
    try {
        simpleSystem = SimpleCTSystem::fromCTSystem(system);
    } catch (const std::exception& e) {
        qInfo() << e.what();
    }
    qInfo() << "Could convert:" << ok;
    qInfo().noquote() << simpleSystem.overview();


    // Creation #2 - using the ctor
    simpleSystem = SimpleCTSystem(FlatPanelDetector(QSize(500, 400), QSizeF(1.0, 1.0)),
                                  CarmGantry(),
                                  XrayTube());
    // --> uses references instead of pointers to make sure we have actual objects (ie. no nullptr)

    // in a SimpleCTSystem, we can no longer add further components (except for beam modifiers)
    // simpleSystem.addComponent(new XrayTube);  // impossible
    // static_cast<CTSystem&>(simpleSystem).addComponent(new XrayTube); // abuse


    // Creation #3 - from blueprints -> tutorial5b()

    // Creation #4 - deserialization -> tutorial5d()
}

void tutorial5b()
{
    // understanding system blueprints
    //auto bpSystem = CTSystemBuilder::createFromBlueprint(blueprints::GenericCarmCT());
    auto bpSystem = makeSimpleCTSystem<blueprints::GenericCarmCT>();

    qInfo().noquote() << bpSystem->overview();
    qInfo().noquote() << bpSystem->info();

    // this is the same as assembly via:
    SimpleCTSystem theSameSystem(FlatPanelDetector(QSize(1280, 960), QSizeF(0.25, 0.25)),
                                 CarmGantry(),
                                 XrayTube(QSize(1.0, 1.0)));

    qInfo().noquote() << theSameSystem.info();

    // manipulating SimpleCTSystem
    // we can replace individual components by others

    // Example #1 - C-arm system with a monoenergetic source
    auto systemEx1 = makeSimpleCTSystem<blueprints::GenericCarmCT>();
    systemEx1->replaceSource(new XrayLaser(75.0, 0.1));


    // Example #2 - tubular system with a flat-panel detector
    auto systemEx2 = makeSimpleCTSystem<blueprints::GenericTubularCT>();
    systemEx2->replaceDetector(new FlatPanelDetector(QSize(1000, 64), QSizeF(1.0, 1.0)));


    // Example #3 - change otherwise fixed values (e.g. detector pixels in C-arm system)
    auto systemEx3 = makeSimpleCTSystem<blueprints::GenericCarmCT>();
    systemEx3->replaceDetector(new FlatPanelDetector(QSize(50, 50), QSizeF(10.0, 10.0)));
}

void tutorial5c()
{
    // recalling what we normally did:
    AcquisitionSetup setup(makeCTSystem<blueprints::GenericCarmCT>(), 10);
    setup.applyPreparationProtocol(protocols::ShortScanTrajectory(750.0f));

    // we can store the full setup in a file to reload it at any later point
    JsonSerializer().serialize(setup, "simpleSetup.json");

    // even more useful for complex comfigurations
    AcquisitionSetup setup2(makeCTSystem<blueprints::GenericCarmCT>(), 100);
    setup2.system()->replaceDetector(new FlatPanelDetector(QSize(100, 100), QSizeF(5.0, 5.0)));
    setup2.system()->source()->setFluxModifier(0.1);
    setup2.applyPreparationProtocol(protocols::FlyingFocalSpot::twoAlternatingSpots({  10.0, 0.0, 0.0 },
                                                                                    { -10.0, 0.0, 0.0}));
    setup2.applyPreparationProtocol(protocols::CirclePlusLineTrajectory(200.0_deg, 750.0f, 200.0));
    JsonSerializer().serialize(setup2, "complexSetup.json");
}

void tutorial5d()
{
    // this is how to load a setup
    auto loadedSetup = JsonSerializer().deserializeAcquisitionSetup("complexSetup.json");
        // or:
        // auto loadedSetup = JsonSerializer().deserialize<AcquisitionSetup>("complexSetup.json");

    // setup is ready to be used with a projector
    auto proj = StandardPipeline().configureAndProject(*loadedSetup,
                                                       SpectralVolumeData::ball(50.0, 0.5, 1.0,
                                                       attenuationModel(database::Composite::Blood)));

    gui::plot(*loadedSetup);
    gui::plot(proj);
}
