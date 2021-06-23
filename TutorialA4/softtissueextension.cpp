#include "softtissueextension.h"

#include "customvolumefilters.h"
#include "models/datamodels1d.h"

DECLARE_SERIALIZABLE_TYPE(SoftTissueExtension)

SoftTissueExtension::SoftTissueExtension(float threshold)
    : m_thresh(threshold)
{
}

bool SoftTissueExtension::isLinear() const
{
    return false;
}

CTL::ProjectionData SoftTissueExtension::project(const CTL::VolumeData& volume)
{
    auto model = std::make_shared<CTL::IdentityModel>()
            * std::make_shared<CTL::StepFunctionModel>(m_thresh, 1.0f, CTL::StepFunctionModel::RightIsZero);

    auto softTissueVolume = volume;
    ModelApplicationFilter(model).filter(softTissueVolume);

    auto projections = ProjectorExtension::project(softTissueVolume);

    return projections;

    // brain shrinking example:
    /*
    auto model = std::make_shared<CTL::IdentityModel>()
            * std::make_shared<CTL::RectFunctionModel>(m_thresh, m_thresh + 0.01f, 1.0f);

    auto brainVolume = volume;
    ModelApplicationFilter(model).filter(brainVolume);

    auto remainingPart = volume - brainVolume;

    brainVolume.setVoxelSize(brainVolume.voxelSize().x * 0.5f);

    return ProjectorExtension::project(brainVolume) + ProjectorExtension::project(remainingPart);
    */
}

CTL::ProjectionData SoftTissueExtension::projectComposite(const CTL::CompositeVolume&)
{
    qWarning("SoftTissueExtension does not support composite volumes");

    return CTL::ProjectionData::dummy();
}

CTL::ProjectionData SoftTissueExtension::projectSparse(const CTL::SparseVoxelVolume&)
{
    qWarning("SoftTissueExtension does not support sparse volumes");

    return CTL::ProjectionData::dummy();
}

QVariant SoftTissueExtension::parameter() const
{
    auto parMap = ProjectorExtension::parameter().toMap();

    parMap.insert("threshold", m_thresh);

    return parMap;
}

void SoftTissueExtension::setParameter(const QVariant& parameter)
{
    ProjectorExtension::setParameter(parameter);

    const auto parMap = parameter.toMap();

    if(parMap.contains("threshold"))
        m_thresh = parMap.value("threshold").toFloat();
}
