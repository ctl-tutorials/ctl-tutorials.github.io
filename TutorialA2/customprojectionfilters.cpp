#include "customprojectionfilters.h"

#include "img/projectiondata.h"
#include "processing/range.h"

#include <QDebug>

DECLARE_SERIALIZABLE_TYPE(MaskingFilter)
DECLARE_SERIALIZABLE_TYPE(LinearExtrapolationFilter)

MaskingFilter::MaskingFilter(const QRect& mask)
    : m_border(mask)
{
}

void MaskingFilter::filter(CTL::ProjectionData& projections)
{
    // loop over all view
    for(auto& view : projections.data())
    {
        // loop over all modules
        for(auto& module : view.data())
        {
            for(uint ch = 0; ch < projections.dimensions().nbChannels; ++ch)
                for(uint row = 0; row < projections.dimensions().nbRows; ++row)
                    if(!m_border.contains(ch, row))
                        module(ch, row) = 0.0f;
        }
    }
}

QVariant MaskingFilter::parameter() const
{
    auto parMap = CTL::AbstractProjectionFilter::parameter().toMap();

    QVariantList list{ m_border.x(), m_border.y(), m_border.width(), m_border.height() };
    parMap.insert("mask border", list);

    return parMap;
}

void MaskingFilter::setParameter(const QVariant &parameter)
{
    CTL::AbstractProjectionFilter::setParameter(parameter);

    const auto parMap = parameter.toMap();
    if(parMap.contains("mask border"))
    {
        auto rectValues = parMap.value("mask border").toList();
        m_border = QRect(rectValues.at(0).toInt(), rectValues.at(1).toInt(),
                         rectValues.at(2).toInt(), rectValues.at(3).toInt());
    }
}

LinearExtrapolationFilter::LinearExtrapolationFilter(const QRect &mask)
    : m_border(mask)
{
}

void LinearExtrapolationFilter::filter(CTL::ProjectionData& projections)
{
    QRect projExtent(0, 0, projections.viewDimensions().nbChannels, projections.viewDimensions().nbRows);
    if(!projExtent.contains(m_border))
    {
        qCritical() << "Could not apply LinearExtrapolationFilter. Filter borders are (partially) "
                       "outside projection data extent.";
        return;
    }

    const auto pixLeft   = static_cast<uint>(m_border.left());
    const auto pixRight  = projections.viewDimensions().nbChannels - static_cast<uint>(m_border.right());
    const auto pixTop    = static_cast<uint>(m_border.top());
    const auto pixBottom = projections.viewDimensions().nbRows - static_cast<uint>(m_border.bottom());

    // loop over all views
    for(auto& view : projections.data())
    {
        // loop over all modules
        for(auto& module : view.data())
        {
            // interpolate the channels
            for(uint row = 0; row < module.dimensions().height; ++row)
            {
                // left side
                auto interpolatedData = CTL::Range<float>::linspace(0.0f, module(m_border.left(), row), pixLeft);
                for(uint ch = 0; ch < pixLeft; ++ch)
                    module(ch, row) = interpolatedData[ch];

                // right side
                interpolatedData = CTL::Range<float>::linspace(module(m_border.right(), row), 0.0f, pixRight);
                for(uint ch = 0; ch < pixRight; ++ch)
                    module(m_border.right() + ch, row) = interpolatedData[ch];
            }

            // interpolate the rows
            for(uint ch = 0; ch < module.dimensions().width; ++ch)
            {
                // top side
                auto interpolatedData = CTL::Range<float>::linspace(0.0f, module(ch, m_border.top()), pixTop);
                for(uint row = 0; row < pixTop; ++row)
                    module(ch, row) = interpolatedData[row];

                // bottom side
                interpolatedData = CTL::Range<float>::linspace(module(ch, m_border.bottom()), 0.0f, pixBottom);
                for(uint row = 0; row < pixBottom; ++row)
                    module(ch, m_border.bottom() + row) = interpolatedData[row];
            }
        }
    }
}

QVariant LinearExtrapolationFilter::parameter() const
{
    auto parMap = CTL::AbstractProjectionFilter::parameter().toMap();

    QVariantList list{ m_border.x(), m_border.y(), m_border.width(), m_border.height() };
    parMap.insert("mask border", list);

    return parMap;
}

void LinearExtrapolationFilter::setParameter(const QVariant &parameter)
{
    CTL::AbstractProjectionFilter::setParameter(parameter);

    const auto parMap = parameter.toMap();
    if(parMap.contains("mask border"))
    {
        auto rectValues = parMap.value("mask border").toList();
        m_border = QRect(rectValues.at(0).toInt(), rectValues.at(1).toInt(),
                         rectValues.at(2).toInt(), rectValues.at(3).toInt());
    }
}
