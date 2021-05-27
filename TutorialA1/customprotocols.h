#ifndef CUSTOMPROTOCOLS_H
#define CUSTOMPROTOCOLS_H

#include "acquisition/abstractpreparestep.h"

class TubeVoltageModulation : public CTL::AbstractPreparationProtocol
{
public:
    TubeVoltageModulation(std::vector<double> voltages);

    // AbstractPreparationProtocol interface
    std::vector<std::shared_ptr<CTL::AbstractPrepareStep>> prepareSteps(uint viewNb, const CTL::AcquisitionSetup& setup) const override;
    bool isApplicableTo(const CTL::AcquisitionSetup &setup) const override;

    // factory methods
    static TubeVoltageModulation singleSwitch(double firstVoltage, double secondVoltage, uint switchView, uint fullNbOfViews);

private:
    std::vector<double> m_voltages;
};

class TubeVoltageModulationFromModel : public CTL::AbstractPreparationProtocol
{
public:
    TubeVoltageModulationFromModel(std::shared_ptr<CTL::AbstractDataModel> model);

    // AbstractPreparationProtocol interface
    std::vector<std::shared_ptr<CTL::AbstractPrepareStep>> prepareSteps(uint viewNb, const CTL::AcquisitionSetup& setup) const override;
    bool isApplicableTo(const CTL::AcquisitionSetup& setup) const override;

    // factory methods
    static TubeVoltageModulationFromModel singleSwitch(double firstVoltage, double secondVoltage, uint switchView);

private:
    std::shared_ptr<CTL::AbstractDataModel> m_model;
};

#endif // CUSTOMPROTOCOLS_H
