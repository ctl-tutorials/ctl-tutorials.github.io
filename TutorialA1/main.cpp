#include <QApplication>

#include "ctl.h"
#include "ctl_ocl.h"
#include "ctl_qtgui.h"

// custom implementations
#include "customblueprints.h"
#include "customprotocols.h"
#include "custommodels.h"

using namespace CTL;

void useBlueprint();
void useProtocol();
void useModel();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(MessageHandler::qInstaller);
    MessageHandler::instance().blacklistMessageType(QtDebugMsg);

    try {

        //useBlueprint();
        //useProtocol();
        useModel();

    }  catch (std::exception& err) {
        qCritical() << err.what();
    }

    return a.exec();
}


void useBlueprint()
{
    // create a setup using our newly created FlatPanelTubularCT as its system
    AcquisitionSetup setup(makeCTSystem<FlatPanelTubularCT>(), 100);
    setup.applyPreparationProtocol(protocols::HelicalTrajectory(3.6_deg, 1.0, -100.0));

    // visualize the setup
    gui::plot(setup);

    // create some projections of a ball phantom ...
    auto projections = OCL::RayCasterProjector().configureAndProject(setup,
                                                                     VoxelVolume<float>::ball(50.0, 1.0, 1.0));
     // ... and visualize the result
    gui::plot(projections);
}

void useProtocol()
{
    AcquisitionSetup setup(makeCTSystem<FlatPanelTubularCT>(), 100);
    setup.applyPreparationProtocol(protocols::HelicalTrajectory(3.6_deg, 1.0, -50.0));

    // version 1: protocol based on a vector containing all voltages (just like protocols::TubeCurrentModulation)
    std::vector<double> voltages;
    for(uint i = 0; i < setup.nbViews(); ++i)
    {
        if(i < setup.nbViews()/2)
            voltages.push_back(70.0);
        else
            voltages.push_back(120.0);
    }

    auto protocol = TubeVoltageModulation(voltages);
    /*
    */

    // version 2: using convenience factory method instead
    /*
    auto protocol = TubeVoltageModulation::singleSwitch(70.0, 120.0, setup.nbViews()/2, setup.nbViews());
    */

    // version 3: using the protocol that is based on a data model
    // -> note that this no longer needs the total number of views as input
    /*
    auto protocol = TubeVoltageModulationFromModel::singleSwitch(70.0, 120.0, setup.nbViews()/2);
    */

    // version 3b: using our QuadraticFunctionModel to create a protocol with linearly increasing voltage
    /*
    auto protocol = TubeVoltageModulationFromModel(std::make_shared<QuadraticFunctionModel>(0.0, 0.5f, 60.0f));
    */


    // check if protocol is applicable and - if so - apply it to the setup
    qInfo() << protocol.isApplicableTo(setup);
    if(protocol.isApplicableTo(setup))
        setup.applyPreparationProtocol(protocol);

    // we can simply serialize the setup without further considerations on de-/serializability of our protocol
    JsonSerializer().serialize(setup, "mySetup.json");

    // create some projections of a cylinder phantom ...
    auto projections = StandardPipeline().configureAndProject(setup,
                                                              SpectralVolumeData::cylinderZ(50.0, 200.0, 1.0, 1.0,
                                                                                            attenuationModel(database::Composite::Bone_Cortical)));
    // ... and visualize the result
    gui::plot(projections);
}

void useModel()
{
    // create an instance of our newly created QuadraticFunctionModel
    auto model = QuadraticFunctionModel(3.0f, 10.5f, -5.0f);

    // sample and visualize some data from the model
    gui::plot(XYDataSeries::sampledFromModel(model, -10.0, 10.0, 100));

    // use the data model viewer to inspect the model more closely
    gui::plot(model);

    // de-/serialization
    // serialize (ie. 'save') the model into file "myModel.json" ...
    JsonSerializer().serialize(model, "myModel.json");
    // ... and deserialize (ie. 'load') a model from the file "myModel.json"
    auto deserializedModel = JsonSerializer().deserializeDataModel("myModel.json");

    // inspect 'deserializedModel' to verify that it has been deserialized correctly
    gui::plot(*deserializedModel);


    // creating compositions of models
    // -> we want to create a step going from 70 to 120 at position 50
    auto model1 = std::make_shared<ConstantModel>(70.0);
    auto model2 = std::make_shared<StepFunctionModel>(50.0, 50.0);

    // create the sum of both models and visualize the result
    gui::plot(model1 + model2);
    // -> this is what we will use in TubeVoltageModulationFromModel::singleSwitch()
}
