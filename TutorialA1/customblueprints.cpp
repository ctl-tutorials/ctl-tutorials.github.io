#include "customblueprints.h"

#include "components/flatpaneldetector.h"
#include "components/tubulargantry.h"
#include "components/xraytube.h"


CTL::AbstractDetector *FlatPanelTubularCT::detector() const
{
    return new CTL::FlatPanelDetector({ 600, 50 } , {1.0 , 1.0}, "Flat panel tubular style");
}

CTL::AbstractGantry *FlatPanelTubularCT::gantry() const
{
    return new CTL::TubularGantry(1000.0, 600.0);
}

CTL::AbstractSource *FlatPanelTubularCT::source() const
{
    return new CTL::XrayTube({1.5, 1.5}, 123.45, 42.0);
}

QString FlatPanelTubularCT::systemName() const
{
    return "Flat panel tubular CT";
}
