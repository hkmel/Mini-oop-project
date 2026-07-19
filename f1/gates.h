#ifndef GATES_H
#define GATES_H

#include "component.h"
#include "pin.h"

// ==================== گیت AND ====================
class AndGate : public Component {
public:
    AndGate(const QString& id, const QPointF& pos) : Component(id, "AND", pos) {
        addPin(new Pin("in1", QPointF(-20, -10), this, PinDirection::Input));
        addPin(new Pin("in2", QPointF(-20, 10), this, PinDirection::Input));
        addPin(new Pin("out", QPointF(20, 0), this, PinDirection::Output));
    }

    void updateState() override {
        PinState in1 = getPinStateById("in1");
        PinState in2 = getPinStateById("in2");

        if (in1 == PinState::Low || in2 == PinState::Low || in1 == PinState::Floating || in2 == PinState::Floating) {
            setPinStateById("out", PinState::Low);
        }
        else if (in1 == PinState::High && in2 == PinState::High) {
            setPinStateById("out", PinState::High);
        }
    }
};

// ==================== گیت OR (نمونه برای بعد) ====================
class OrGate : public Component {
public:
    OrGate(const QString& id, const QPointF& pos) : Component(id, "OR", pos) {
        addPin(new Pin("in1", QPointF(-20, -10), this, PinDirection::Input));
        addPin(new Pin("in2", QPointF(-20, 10), this, PinDirection::Input));
        addPin(new Pin("out", QPointF(20, 0), this, PinDirection::Output));
    }

    void updateState() override {
        PinState in1 = getPinStateById("in1");
        PinState in2 = getPinStateById("in2");

        if (in1 == PinState::High || in2 == PinState::High) {
            setPinStateById("out", PinState::High);
        }
        else if (in1 == PinState::Low && in2 == PinState::Low) {
            setPinStateById("out", PinState::Low);
        }
        else {
            setPinStateById("out", PinState::Floating);
        }
    }
};

#endif // GATES_H