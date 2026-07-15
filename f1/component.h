#ifndef COMPONENT_H
#define COMPONENT_H
#include<QString>
#include<QPointF>
#include<QVector>
class Pin;
class Component{
public:
    Component(const QString&id,const QString&name,const QPointF&pos);
    virtual~Component();
    QString getId()const;
    QString getName()const;
    QPointF getPosition()const;
    void setPosition(const QPointF&pos);
    int getRotationAngle()const;
    void rotateClockwise();
    const QVector<Pin*>&getPins()const;
    virtual void updateState()=0;
protected:
    void addPin(Pin*pin);
    QString id;
    QString name;
    QPointF position;
    int rotationAngle;
    QVector<Pin*>pins;
};
#endif