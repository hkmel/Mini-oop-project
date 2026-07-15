#include "pin.h"
#include "component.h"
Pin::Pin(const QString& id,const QPointF& offset,Component* parent){
    this->id=id;
    this->offset=offset;
    this->parentComponent=parent;
    this->connectedState=false;
}
Pin::~Pin(){}
QString Pin::getId() const{return id;}
QPointF Pin::getOffset() const{return offset;}
QPointF Pin::getAbsolutePosition() const{
    if(!parentComponent)return offset;
    int angle=parentComponent->getRotationAngle();
    double rx=offset.x();
    double ry=offset.y();
    if(angle==90){rx=-offset.y();ry=offset.x();}
    else if(angle==180){rx=-offset.x();ry=-offset.y();}
    else if(angle==270){rx=offset.y();ry=-offset.x();}
    return parentComponent->getPosition()+QPointF(rx,ry);
}
Component* Pin::getParentComponent() const{return parentComponent;}
void Pin::setConnected(bool connected){this->connectedState=connected;}
bool Pin::isConnected() const{return connectedState;}