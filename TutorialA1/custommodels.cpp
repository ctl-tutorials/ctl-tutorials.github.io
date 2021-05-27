#include "custommodels.h"

DECLARE_SERIALIZABLE_TYPE(QuadraticFunctionModel)

QuadraticFunctionModel::QuadraticFunctionModel(float a, float b, float c)
    : m_a(a)
    , m_b(b)
    , m_c(c)
{
}

// f(x) = ax² + bx + c
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
    auto parameters = CTL::AbstractDataModel::parameter().toMap();

    parameters.insert("a", m_a);
    parameters.insert("b", m_b);
    parameters.insert("c", m_c);

    return parameters;
}

void QuadraticFunctionModel::setParameter(const QVariant &parameter)
{
    CTL::AbstractDataModel::setParameter(parameter);

    auto parMap = parameter.toMap();
    if(parMap.contains("a"))
        m_a = parMap.value("a").toFloat();
    if(parMap.contains("b"))
        m_b = parMap.value("b").toFloat();
    if(parMap.contains("c"))
        m_c = parMap.value("c").toFloat();
}
