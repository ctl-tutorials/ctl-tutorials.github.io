#include "customprotocols.h"

#include "acquisition/acquisitionsetup.h"
#include "acquisition/preparesteps.h"
#include "models/abstractdatamodel.h"
#include "models/datamodels1d.h"

TubeVoltageModulation::TubeVoltageModulation(std::vector<double> voltages)
    : m_voltages(std::move(voltages))
{
}

std::vector<std::shared_ptr<CTL::AbstractPrepareStep>> TubeVoltageModulation::prepareSteps(uint viewNb, const CTL::AcquisitionSetup&) const
{
    // create the prepare steps for all components we need to prepare with our protocol
    // -> here, we only want to modify the tube voltage of an XrayTube component
    auto prepareStep = CTL::prepare::XrayTubeParam::forTubeVoltage(m_voltages[viewNb]);

    auto prepareVector = std::vector<std::shared_ptr<CTL::AbstractPrepareStep>> { std::move(prepareStep) };
    // ... here, you could append any futher prepare steps that are needed for your protocol

    return prepareVector;
}

bool TubeVoltageModulation::isApplicableTo(const CTL::AcquisitionSetup& setup) const
{
    // we can utilize the fact that the prepare steps already provide an "isApplicableTo()" routine
    // -> we can use it to verify that the prepate step we need (ie. XrayTubeParam) can be applied to 'setup'
    CTL::prepare::XrayTubeParam dummy;

    // we also check if the vector of voltages contains the same number of elements as there are views in 'setup'
    return dummy.isApplicableTo(*setup.system()) && (m_voltages.size() == setup.nbViews());
}

TubeVoltageModulation TubeVoltageModulation::singleSwitch(double firstVoltage, double secondVoltage, uint switchView, uint fullNbOfViews)
{
    // we create the vector of voltages and populate it with the desired values
    std::vector<double> voltages(fullNbOfViews);
    for(uint i = 0; i < fullNbOfViews; ++i)
    {
        if(i < switchView)
            voltages[i] = firstVoltage;
        else
            voltages[i] = secondVoltage;
    }

    // we create a TubeVoltageModulation protocol from the vector and return it
    return TubeVoltageModulation(std::move(voltages));
}

TubeVoltageModulationFromModel::TubeVoltageModulationFromModel(std::shared_ptr<CTL::AbstractDataModel> model)
    : m_model(std::move(model))
{
}

std::vector<std::shared_ptr<CTL::AbstractPrepareStep>> TubeVoltageModulationFromModel::prepareSteps(uint viewNb,
                                                                                                    const CTL::AcquisitionSetup&) const
{
    // here, we replace the lookup in the vector of voltages by the command to sample the value from the data model
    auto prepareStep = CTL::prepare::XrayTubeParam::forTubeVoltage(m_model->valueAt(float(viewNb)));

    auto prepareVector = std::vector<std::shared_ptr<CTL::AbstractPrepareStep>> { std::move(prepareStep) };

    return prepareVector;
}

bool TubeVoltageModulationFromModel::isApplicableTo(const CTL::AcquisitionSetup& setup) const
{
    CTL::prepare::XrayTubeParam dummy;

    // note that we now check that our data model is not a nullptr
    return dummy.isApplicableTo(*setup.system()) && (m_model != nullptr);
}

TubeVoltageModulationFromModel TubeVoltageModulationFromModel::singleSwitch(double firstVoltage, double secondVoltage, uint switchView)
{
    // note that we no longer need the total number of views here

    auto model1 = std::make_shared<CTL::ConstantModel>(firstVoltage);
    auto model2 = std::make_shared<CTL::StepFunctionModel>(float(switchView), secondVoltage - firstVoltage);

    return TubeVoltageModulationFromModel(model1 + model2);
}
