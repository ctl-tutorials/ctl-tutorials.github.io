#ifndef CUSTOMPROJECTIONFILTERS_H
#define CUSTOMPROJECTIONFILTERS_H

#include "processing/abstractprojectionfilter.h"

#include <QRect>

// MaskingFilter
class MaskingFilter : public CTL::AbstractProjectionFilter
{
    CTL_TYPE_ID(CTL::AbstractProjectionFilter::UserType + 2)

public:
    explicit MaskingFilter(const QRect& mask);

    // AbstractProjectionFilter interface
    void filter(CTL::ProjectionData& projections) override;

    // de-/serialization
    QVariant parameter() const override;
    void setParameter(const QVariant &parameter) override;

private:
    MaskingFilter() = default;

    QRect m_border;

};

// LinearExtrapolationFilter
class LinearExtrapolationFilter : public CTL::AbstractProjectionFilter
{
    CTL_TYPE_ID(CTL::AbstractProjectionFilter::UserType + 3)

public:
    explicit LinearExtrapolationFilter(const QRect& mask);

    // AbstractProjectionFilter interface
    void filter(CTL::ProjectionData& projections) override;

    // de-/serialization
    QVariant parameter() const override;
    void setParameter(const QVariant &parameter) override;

private:
    LinearExtrapolationFilter() = default;

    QRect m_border;

};

#endif // CUSTOMPROJECTIONFILTERS_H
