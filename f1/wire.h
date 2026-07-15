#ifndef WIRE_H
#define WIRE_H
#include "pin.h"
class Wire{
public:
    Wire(Pin* p1,Pin* p2);
    ~Wire();
    Pin* getPin1() const;
    Pin* getPin2() const;
    bool containsPin(Pin* pin) const;
private:
    Pin* pin1;
    Pin* pin2;
};
#endif