
#ifndef BASECOMPONENTS_H

#define BASECOMPONENTS_H

#include"component.h"

class GndComponent:public Component{

public:

    GndComponent(const QString&id,const QPointF&pos);

    void updateState()override;

};

class ResistorComponent:public Component{

public:

    ResistorComponent(const QString&id,const QPointF&pos,double resistance);

    double getResistance()const;

    void setResistance(double r);

    void updateState()override;

private:

    double resistance;

};

class CapacitorComponent:public Component{

public:

    CapacitorComponent(const QString&id,const QPointF&pos,double capacitance);

    double getCapacitance()const;

    void setCapacitance(double c);

    void updateState()override;

private:

    double capacitance;

};

class InductorComponent:public Component{

public:

    InductorComponent(const QString&id,const QPointF&pos,double inductance);

    double getInductance()const;

    void setInductance(double l);

    void updateState()override;

private:

    double inductance;

};

class VoltageSourceComponent:public Component{

public:

    VoltageSourceComponent(const QString&id,const QPointF&pos,double voltage);

    double getVoltage()const;

    void setVoltage(double v);

    void updateState()override;

private:

    double voltage;

};

class SwitchComponent:public Component{

public:

    SwitchComponent(const QString&id,const QPointF&pos);

    bool isOpen()const;

    void toggle();

    void updateState()override;

private:

    bool openState;

};

class ButtonComponent:public Component{

public:

    ButtonComponent(const QString&id,const QPointF&pos);

    bool isPressed()const;

    void press();

    void release();

    void updateState()override;

private:

    bool pressedState;

};

class AndGateComponent:public Component{

public:

    AndGateComponent(const QString&id,const QPointF&pos);

    void updateState()override;

};

class OrGateComponent:public Component{

public:

    OrGateComponent(const QString&id,const QPointF&pos);

    void updateState()override;

};

class NotGateComponent:public Component{

public:

    NotGateComponent(const QString&id,const QPointF&pos);

    void updateState()override;

};

class NandGateComponent:public Component{

public:

    NandGateComponent(const QString&id,const QPointF&pos);

    void updateState()override;

};

class XorGateComponent:public Component{

public:

    XorGateComponent(const QString&id,const QPointF&pos);

    void updateState()override;

};

class DFlipFlopComponent:public Component{

public:

    DFlipFlopComponent(const QString&id,const QPointF&pos);

    void updateState()override;

private:

    bool lastClk;

    bool stateQ;

};

class LedComponent:public Component{

public:

    LedComponent(const QString&id,const QPointF&pos);

    bool isOn()const;

    void updateState()override;

private:

    bool onState;

};

class SevenSegmentComponent:public Component{

public:

    SevenSegmentComponent(const QString&id,const QPointF&pos);

    void updateState()override;

    int getDisplayedDigit()const;

private:

    int digit;

};

#endif