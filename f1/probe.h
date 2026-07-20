#ifndef PROBE_H
#define PROBE_H

#include "component.h"
#include "pin.h"

class VoltageProbe : public Component {
public:
    VoltageProbe(const QString& id, const QPointF& pos)
        : Component(id, "VoltageProbe", pos), measuredVoltage(0.0), currentPinState(PinState::Floating)
    {
        // پروب فقط یک پین ورودی برای اتصال به سیم یا پین قطعه دارد
        addPin(new Pin("in", QPointF(-25, 0), this, PinDirection::Input));
    }

    void updateState() override {
        currentPinState = getPinStateById("in");

        // تبدیل وضعیت منطقی به ولتاژ تقریبی مدار دیجیتال
        if (currentPinState == PinState::High) {
            measuredVoltage = 5.0; // ۵ ولت منطقی
        } else if (currentPinState == PinState::Low) {
            measuredVoltage = 0.0; // صفر ولت (GND)
        } else {
            measuredVoltage = 0.0; // وضعیت نامشخص / Floating
        }
    }

    double getMeasuredVoltage() const { return measuredVoltage; }
    PinState getMeasuredState() const { return currentPinState; }

private:
    double measuredVoltage;
    PinState currentPinState;
};

#endif // PROBE_H