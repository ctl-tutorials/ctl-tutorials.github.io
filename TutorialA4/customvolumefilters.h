#ifndef CUSTOMVOLUMEFILTERS_H
#define CUSTOMVOLUMEFILTERS_H

#include "models/abstractdatamodel.h"
#include "processing/abstractvolumefilter.h"

// MovingAverageFilter
class MovingAverageFilter : public CTL::AbstractVolumeFilter
{
    CTL_TYPE_ID(CTL::AbstractVolumeFilter::UserType + 4)

public:
    // AbstractVolumeFilter interface
    void filter(CTL::VoxelVolume<float> &volume) override;

    // de-/serialization
    // -> no custom implementations needed, since class has no additional members
};

// ModelApplicationFilter
class ModelApplicationFilter : public CTL::AbstractVolumeFilter
{
    CTL_TYPE_ID(CTL::AbstractVolumeFilter::UserType + 5)

public:
    ModelApplicationFilter(std::shared_ptr<CTL::AbstractDataModel> model);

    // AbstractVolumeFilter interface
    void filter(CTL::VoxelVolume<float> &volume) override;

    // de-/serialization
    QVariant parameter() const override;
    void setParameter(const QVariant &parameter) override;

private:
    ModelApplicationFilter() = default;

    std::shared_ptr<CTL::AbstractDataModel> m_model;
};

#endif // CUSTOMVOLUMEFILTERS_H
