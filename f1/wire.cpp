#include "wire.h"
Wire::Wire(Pin* p1,Pin* p2){
    this->pin1=p1;
    this->pin2=p2;
    if(this->pin1)this->pin1->setConnected(true);
    if(this->pin2)this->pin2->setConnected(true);
}
Wire::~Wire(){
    if(pin1)pin1->setConnected(false);
    if(pin2)pin2->setConnected(false);
}
Pin* Wire::getPin1() const{return pin1;}
Pin* Wire::getPin2() const{return pin2;}
bool Wire::containsPin(Pin* pin) const{
    return (pin1==pin||pin2==pin);
}