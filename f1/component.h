#ifndef COMPONENT_H
#define COMPONENT_H

#include <QString>
#include <QPointF>
#include <QVector>
#include "pin.h"

class Component {
public:
    Component(const QString& id, const QString& name, const QPointF& pos);
    virtual ~Component();

    QString getId() const;
    void setId(const QString& newId) { id = newId; } // 🌟 متد جدید برای تغییر نام/شناسه قطعه
    QString getName() const;
    QPointF getPosition() const;
    void setPosition(const QPointF& pos);
    int getRotationAngle() const;
    void rotateClockwise();
    const QVector<Pin*>& getPins() const;

    QString getType() const { return name; }

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
};

#endif // COMPONENT_H