#include "custommodels.h"
#include "io/serializationhelper.h"

DECLARE_SERIALIZABLE_TYPE(QuadraticFunctionModel)

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
