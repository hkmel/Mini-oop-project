#include"component.h"
#include"pin.h"
Component::Component(const QString&id,const QString&name,const QPointF&pos){
    this->id=id;
    this->name=name;
    this->position=pos;
    this->rotationAngle=0;
}
Component::~Component(){
    qDeleteAll(pins);
}
QString Component::getId()const{return id;}
QString Component::getName()const{return name;}
QPointF Component::getPosition()const{return position;}
void Component::setPosition(const QPointF&pos){position=pos;}
int Component::getRotationAngle()const{return rotationAngle;}
void Component::rotateClockwise(){
    rotationAngle=(rotationAngle+90)%360;
}
const QVector<Pin*>&Component::getPins()const{return pins;}
void Component::addPin(Pin*pin){pins.append(pin);}