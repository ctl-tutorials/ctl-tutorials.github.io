#include "customvolumefilters.h"

#include "img/voxelvolume.h"
#include "processing/neighborselector.h"

DECLARE_SERIALIZABLE_TYPE(MovingAverageFilter)
DECLARE_SERIALIZABLE_TYPE(ModelApplicationFilter)

void MovingAverageFilter::filter(CTL::VoxelVolume<float>& volume)
{
    const auto volCopy = volume;
    CTL::NeighborSelector selector(volCopy);

    for(uint x = 0; x < volume.dimensions().x; ++x)
        for(uint y = 0; y < volume.dimensions().y; ++y)
            for(uint z = 0; z < volume.dimensions().z; ++z)
            {
                const auto values = selector.voxelAndNeighborsMirrored(x, y, z);
                const auto sum = std::accumulate(values.cbegin(), values.cend(), 0.0f);

                volume(x, y, z) = sum / float(values.size());
            }
}

ModelApplicationFilter::ModelApplicationFilter(std::shared_ptr<CTL::AbstractDataModel> model)
    : m_model(std::move(model))
{
}

void ModelApplicationFilter::filter(CTL::VoxelVolume<float>& volume)
{
    for(auto& voxel : volume)
        voxel = m_model->valueAt(voxel);
}

QVariant ModelApplicationFilter::parameter() const
{
    auto parMap = CTL::AbstractVolumeFilter::parameter().toMap();

    parMap.insert("model", m_model->toVariant());

    return parMap;
}

void ModelApplicationFilter::setParameter(const QVariant& parameter)
{
    CTL::AbstractVolumeFilter::setParameter(parameter);

    const auto parMap = parameter.toMap();

    if(parMap.contains("model"))
    {
        m_model.reset(CTL::SerializationHelper::parseDataModel(parMap.value("model")));
    }
}

