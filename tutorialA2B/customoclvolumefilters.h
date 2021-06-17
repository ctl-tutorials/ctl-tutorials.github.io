#ifndef CUSTOMOCLVOLUMEFILTERS_H
#define CUSTOMOCLVOLUMEFILTERS_H

#include "processing/genericoclvolumefilter.h"

class VolumeSegmentationFilter : public CTL::OCL::GenericOCLVolumeFilter
{
    CTL_TYPE_ID(CTL::OCL::GenericOCLVolumeFilter::UserType + 100)

    // AbstractVolumeFilter interface
public:
    VolumeSegmentationFilter(std::vector<float> thresholds);

    void filter(CTL::VoxelVolume<float> &volume) override;
    QVariant parameter() const override;
    void setParameter(const QVariant &parameter) override;

private:
    VolumeSegmentationFilter();

    std::vector<float> m_thresholds;
};

#endif // CUSTOMOCLVOLUMEFILTERS_H
