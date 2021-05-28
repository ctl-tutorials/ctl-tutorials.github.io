#ifndef CUSTOMBLUEPRINTS_H
#define CUSTOMBLUEPRINTS_H

#include "acquisition/systemblueprints.h"

class FlatPanelTubularCT : public CTL::AbstractCTSystemBlueprint
{
    // AbstractCTSystemBlueprint interface
public:
    CTL::AbstractDetector *detector() const override;
    CTL::AbstractGantry *gantry() const override;
    CTL::AbstractSource *source() const override;
    QString systemName() const override;
};


#endif // CUSTOMBLUEPRINTS_H
