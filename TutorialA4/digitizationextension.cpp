#include "digitizationextension.h"

#include "custommodels.h"

DECLARE_SERIALIZABLE_TYPE(DigitizationExtension)

DigitizationExtension::DigitizationExtension(float maxValue, uint bitDepth)
    : m_maxValue(maxValue)
    , m_bitDepth(bitDepth)
{
}

bool DigitizationExtension::isLinear() const
{
    return false;
}

CTL::ProjectionData DigitizationExtension::extendedProject(const MetaProjector& nestedProjector)
{
    // compute the (clean) projections
    auto projections = nestedProjector.project();

    // (optional)
    emit notifier()->information("Processing digitization effect.");

    // process the data
    auto model = std::make_shared<DiscretizingModel>(0.0f, m_maxValue, std::pow(2.0f, static_cast<float>(m_bitDepth)));
    for(auto& pix : projections)
        pix = model->valueAt(pix);

    return projections;
}

QVariant DigitizationExtension::parameter() const
{
    auto parMap = ProjectorExtension::parameter().toMap();

    parMap.insert("maximum value", m_maxValue);
    parMap.insert("bit depth", m_bitDepth);

    return parMap;
}

void DigitizationExtension::setParameter(const QVariant& parameter)
{
    ProjectorExtension::setParameter(parameter);

    const auto parMap = parameter.toMap();

    if(parMap.contains("maximum value"))
        m_maxValue = parMap.value("maximum value").toFloat();
    if(parMap.contains("bit depth"))
        m_bitDepth = parMap.value("bit depth").toUInt();
}
