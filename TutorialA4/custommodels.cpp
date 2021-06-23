#include "custommodels.h"
#include "io/serializationhelper.h"

DECLARE_SERIALIZABLE_TYPE(QuadraticFunctionModel)
DECLARE_SERIALIZABLE_TYPE(DiscretizingModel)

QuadraticFunctionModel::QuadraticFunctionModel(float a, float b, float c)
    : m_a(a)
    , m_b(b)
    , m_c(c)
{
}

float QuadraticFunctionModel::valueAt(float position) const
{
    return m_a * position * position + m_b * position + m_c;
}

CTL::AbstractDataModel* QuadraticFunctionModel::clone() const
{
    return new QuadraticFunctionModel(*this);
}

QVariant QuadraticFunctionModel::parameter() const
{
    auto parMap = AbstractDataModel::parameter().toMap();
    parMap.insert("a", m_a);
    parMap.insert("b", m_b);
    parMap.insert("c", m_c);

    return parMap;
}

void QuadraticFunctionModel::setParameter(const QVariant& parameter)
{
    AbstractDataModel::setParameter(parameter);

    const auto parMap = parameter.toMap();

    if(parMap.contains("a"))
        m_a = parMap.value("a").toFloat();
    if(parMap.contains("b"))
        m_b = parMap.value("b").toFloat();
    if(parMap.contains("c"))
        m_c = parMap.value("c").toFloat();
}


DiscretizingModel::DiscretizingModel(float minValue, float maxValue, uint nbValues)
    : m_minValue(minValue)
    , m_maxValue(maxValue)
    , m_nbValues(nbValues)
{

}

float DiscretizingModel::valueAt(float position) const
{
    // border case guards
    if(position < m_minValue)
        return m_minValue;
    if(position > m_maxValue)
        return m_maxValue;

    const auto stepwidth = (m_maxValue - m_minValue) / double(m_nbValues);

    const auto binIndex = std::floor((position - m_minValue) / stepwidth);

    return static_cast<float>(binIndex * stepwidth) + m_minValue;
}

CTL::AbstractDataModel* DiscretizingModel::clone() const
{
    return new DiscretizingModel(*this);
}

QVariant DiscretizingModel::parameter() const
{
    auto parMap = AbstractDataModel::parameter().toMap();

    parMap.insert("min value", m_minValue);
    parMap.insert("max value", m_maxValue);
    parMap.insert("number values", m_nbValues);

    return parMap;
}

void DiscretizingModel::setParameter(const QVariant& parameter)
{
    AbstractDataModel::setParameter(parameter);

    const auto parMap = parameter.toMap();

    if(parMap.contains("min value"))
        m_minValue = parMap.value("min value").toFloat();
    if(parMap.contains("max value"))
        m_maxValue = parMap.value("max value").toFloat();
    if(parMap.contains("number values"))
        m_nbValues = parMap.value("number values").toUInt();
}
