
#ifndef COMPONENT_H

#define COMPONENT_H



#include <QString>

#include <QPointF>

#include <QVector>

#include "pin.h"



class Component {

public:

    // سازنده به‌روزرسانی‌شده با مقدار و یکای پیش‌فرض

    Component(const QString& id, const QString& name, const QPointF& pos, double value = 0.0, const QString& unit = "");

    virtual ~Component();



    QString getId() const;

    void setId(const QString& newId) { id = newId; } // متد تغییر نام/شناسه قطعه



    QString getName() const;

    QPointF getPosition() const;

    void setPosition(const QPointF& pos);



    int getRotationAngle() const;

    void rotateClockwise();

    const QVector<Pin*>& getPins() const;



    QString getType() const { return name; }



    // 🌟 متدهای جدید برای مدیریت مقدار و یکای استاندارد

    double getValue() const { return value; }

    void setValue(double newValue) { value = newValue; }



    QString getUnit() const { return unit; }

    void setUnit(const QString& newUnit) { unit = newUnit; }



    virtual void updateState() = 0;



    PinState getPinStateById(const QString& pinId) const {

        for (Pin* p : pins) {

            if (p->getId() == pinId) return p->getState();

        }

        return PinState::Floating;

    }



    void setPinStateById(const QString& pinId, PinState newState) {

        for (Pin* p : pins) {

            if (p->getId() == pinId) {

                p->setState(newState);

                return;

            }

        }

    }



protected:

    void addPin(Pin* pin);

    QString id;

    QString name;

    QPointF position;

    int rotationAngle;

    QVector<Pin*> pins;



    // 🌟 متغیرهای اضافه شده برای مقدار و یکا

    double value;

    QString unit;

};



#endif // COMPONENT_H