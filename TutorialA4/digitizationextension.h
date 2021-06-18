#ifndef DIGITIZATIONEXTENSION_H
#define DIGITIZATIONEXTENSION_H

#include <projectors/projectorextension.h>

class DigitizationExtension : public CTL::ProjectorExtension
{
    CTL_TYPE_ID(CTL::ProjectorExtension::UserType + 200)

public:
    DigitizationExtension(float maxValue, uint bitDepth);

    bool isLinear() const override;
    QVariant parameter() const override;
    void setParameter(const QVariant &parameter) override;

protected:
    CTL::ProjectionData extendedProject(const MetaProjector& nestedProjector) override;

private:
    DigitizationExtension() = default;

    float m_maxValue;
    uint m_bitDepth;

};

#endif // DIGITIZATIONEXTENSION_H
