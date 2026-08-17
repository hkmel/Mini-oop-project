#include "basecomponents.h"
#include "pin.h"

GndComponent::GndComponent(const QString&id,const QPointF&pos):Component(id,"GND",pos){
    addPin(new Pin("GND",QPointF(0,10),this));
}
void GndComponent::updateState(){}

ResistorComponent::ResistorComponent(const QString&id,const QPointF&pos,double resistance):Component(id,"Resistor",pos){
    this->resistance=resistance;
    addPin(new Pin("A",QPointF(-20,0),this));
    addPin(new Pin("B",QPointF(20,0),this));
}
double ResistorComponent::getResistance()const{return resistance;}
void ResistorComponent::setResistance(double r){resistance=r;}
void ResistorComponent::updateState(){}

CapacitorComponent::CapacitorComponent(const QString&id,const QPointF&pos,double capacitance):Component(id,"Capacitor",pos){
    this->capacitance=capacitance;
    addPin(new Pin("A",QPointF(-10,0),this));
    addPin(new Pin("B",QPointF(10,0),this));
}
double CapacitorComponent::getCapacitance()const{return capacitance;}
void CapacitorComponent::setCapacitance(double c){capacitance=c;}
void CapacitorComponent::updateState(){}

InductorComponent::InductorComponent(const QString&id,const QPointF&pos,double inductance):Component(id,"Inductor",pos){
    this->inductance=inductance;
    addPin(new Pin("A",QPointF(-20,0),this));
    addPin(new Pin("B",QPointF(20,0),this));
}
double InductorComponent::getInductance()const{return inductance;}
void InductorComponent::setInductance(double l){inductance=l;}
void InductorComponent::updateState(){}

VoltageSourceComponent::VoltageSourceComponent(const QString&id,const QPointF&pos,double voltage):Component(id,"VoltageSource",pos){
    this->voltage=voltage;
    addPin(new Pin("+",QPointF(0,-20),this));
    addPin(new Pin("-",QPointF(0,20),this));
}
double VoltageSourceComponent::getVoltage()const{return voltage;}
void VoltageSourceComponent::setVoltage(double v){voltage=v;}
void VoltageSourceComponent::updateState(){}

SwitchComponent::SwitchComponent(const QString&id,const QPointF&pos):Component(id,"Switch",pos){
    openState=true;
    addPin(new Pin("A",QPointF(-20,0),this));
    addPin(new Pin("B",QPointF(20,0),this));
}
bool SwitchComponent::isOpen()const{return openState;}
void SwitchComponent::toggle(){openState=!openState;}
void SwitchComponent::updateState(){}

ButtonComponent::ButtonComponent(const QString&id,const QPointF&pos):Component(id,"Button",pos){
    pressedState=false;
    addPin(new Pin("A",QPointF(-20,0),this));
    addPin(new Pin("B",QPointF(20,0),this));
}
bool ButtonComponent::isPressed()const{return pressedState;}
void ButtonComponent::press(){pressedState=true;}
void ButtonComponent::release(){pressedState=false;}
void ButtonComponent::updateState(){}

AndGateComponent::AndGateComponent(const QString&id,const QPointF&pos):Component(id,"AND",pos){
    addPin(new Pin("A",QPointF(-20,-10),this));
    addPin(new Pin("B",QPointF(-20,10),this));
    addPin(new Pin("OUT",QPointF(20,0),this));
}
void AndGateComponent::updateState(){}

OrGateComponent::OrGateComponent(const QString&id,const QPointF&pos):Component(id,"OR",pos){
    addPin(new Pin("A",QPointF(-20,-10),this));
    addPin(new Pin("B",QPointF(-20,10),this));
    addPin(new Pin("OUT",QPointF(20,0),this));
}
void OrGateComponent::updateState(){}

NotGateComponent::NotGateComponent(const QString&id,const QPointF&pos):Component(id,"NOT",pos){
    addPin(new Pin("IN",QPointF(-20,0),this));
    addPin(new Pin("OUT",QPointF(20,0),this));
}
void NotGateComponent::updateState(){}

NandGateComponent::NandGateComponent(const QString&id,const QPointF&pos):Component(id,"NAND",pos){
    addPin(new Pin("A",QPointF(-20,-10),this));
    addPin(new Pin("B",QPointF(-20,10),this));
    addPin(new Pin("OUT",QPointF(20,0),this));
}
void NandGateComponent::updateState(){}

XorGateComponent::XorGateComponent(const QString&id,const QPointF&pos):Component(id,"XOR",pos){
    addPin(new Pin("A",QPointF(-20,-10),this));
    addPin(new Pin("B",QPointF(-20,10),this));
    addPin(new Pin("OUT",QPointF(20,0),this));
}
void XorGateComponent::updateState(){}

DFlipFlopComponent::DFlipFlopComponent(const QString&id,const QPointF&pos):Component(id,"D Flip-Flop",pos){
    lastClk=false;
    stateQ=false;
    addPin(new Pin("D",QPointF(-20,-10),this));
    addPin(new Pin("CLK",QPointF(-20,10),this));
    addPin(new Pin("Q",QPointF(20,-10),this));
    addPin(new Pin("QB",QPointF(20,10),this));
}
void DFlipFlopComponent::updateState(){}

LedComponent::LedComponent(const QString&id,const QPointF&pos):Component(id,"LED",pos){
    onState=false;
    addPin(new Pin("ANODE",QPointF(-15,0),this));
    addPin(new Pin("CATHODE",QPointF(15,0),this));
}
bool LedComponent::isOn()const{return onState;}
void LedComponent::updateState(){}

SevenSegmentComponent::SevenSegmentComponent(const QString&id,const QPointF&pos):Component(id,"7-Segment",pos){
    digit=0;
    addPin(new Pin("A",QPointF(-30,-30),this));
    addPin(new Pin("B",QPointF(-30,-10),this));
    addPin(new Pin("C",QPointF(-30,10),this));
    addPin(new Pin("D",QPointF(-30,30),this));
    addPin(new Pin("E",QPointF(30,-30),this));
    addPin(new Pin("F",QPointF(30,-10),this));
    addPin(new Pin("G",QPointF(30,10),this));
    addPin(new Pin("COM",QPointF(30,30),this));
}
void SevenSegmentComponent::updateState(){}
int SevenSegmentComponent::getDisplayedDigit()const{return digit;}


BatteryComponent::BatteryComponent(const QString& id, const QPointF& pos, double voltage, double internalRes)
    : Component(id, "Battery", pos, voltage, "V") {
    this->internalResistance = internalRes;


    addPin(new Pin("+", QPointF(0, -20), this, PinDirection::Output));
    addPin(new Pin("-", QPointF(0, 20), this, PinDirection::Output));
}

double BatteryComponent::getInternalResistance() const { return internalResistance; }
void BatteryComponent::setInternalResistance(double r) { internalResistance = r; }
void BatteryComponent::updateState() {}

ClockGeneratorComponent::ClockGeneratorComponent(const QString& id, const QPointF& pos, double frequency)
    : Component(id, "Clock Generator", pos, frequency, "Hz") {
    this->logicState = false;


    addPin(new Pin("OUT", QPointF(20, 0), this, PinDirection::Output));
}

bool ClockGeneratorComponent::getCurrentState() const { return logicState; }

void ClockGeneratorComponent::toggleState() {
    logicState = !logicState;

    setPinStateById("OUT", logicState ? PinState::High : PinState::Low);
}

void ClockGeneratorComponent::updateState() {}