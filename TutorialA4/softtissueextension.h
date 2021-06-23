#ifndef SOFTTISSUEEXTENSION_H
#define SOFTTISSUEEXTENSION_H

#include "projectors/projectorextension.h"

class SoftTissueExtension : public CTL::ProjectorExtension
{
    CTL_TYPE_ID(CTL::ProjectorExtension::UserType + 201)

public:
    SoftTissueExtension(float threshold);

    CTL::ProjectionData project(const CTL::VolumeData& volume) override;
    CTL::ProjectionData projectComposite(const CTL::CompositeVolume& volume) override;
    CTL::ProjectionData projectSparse(const CTL::SparseVoxelVolume& volume) override;

    bool isLinear() const override;

    QVariant parameter() const override;
    void setParameter(const QVariant &parameter) override;

private:
    SoftTissueExtension() = default;

    float m_thresh;
};

#endif // SOFTTISSUEEXTENSION_H
