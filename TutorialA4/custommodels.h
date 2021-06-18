#ifndef CUSTOMMODELS_H
#define CUSTOMMODELS_H

#include "models/abstractdatamodel.h"

// f(x) = ax² + bx + c
class QuadraticFunctionModel : public CTL::AbstractDataModel
{
    CTL_TYPE_ID(CTL::AbstractDataModel::UserType + 1);

public:
    QuadraticFunctionModel(float a, float b, float c);

    // AbstractDataModel interface
    float valueAt(float position) const override;
    CTL::AbstractDataModel* clone() const override;

    // de-/serialization
    QVariant parameter() const override;
    void setParameter(const QVariant& parameter) override;

private:
    float m_a = 0.0f;
    float m_b = 0.0f;
    float m_c = 0.0f;

    QuadraticFunctionModel() = default;
};


class DiscretizingModel : public CTL::AbstractDataModel
{
    CTL_TYPE_ID(CTL::AbstractDataModel::UserType + 30);

public:
    DiscretizingModel(float minValue, float maxValue, uint nbValues);

    // AbstractDataModel interface
    float valueAt(float position) const override;
    CTL::AbstractDataModel* clone() const override;

    // de-/serialization
    QVariant parameter() const override;
    void setParameter(const QVariant& parameter) override;

private:
    DiscretizingModel() = default;

    float m_minValue;
    float m_maxValue;
    uint m_nbValues;
};

#endif // CUSTOMMODELS_H
