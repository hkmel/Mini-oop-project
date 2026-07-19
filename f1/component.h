#ifndef COMPONENT_H
#define COMPONENT_H

#include <QString>
#include <QPointF>
#include <QVector>
#include "pin.h" // اضافه شدن هدر پین برای شناختن PinState و PinDirection

class Component {
public:
    Component(const QString& id, const QString& name, const QPointF& pos);
    virtual ~Component();

    QString getId() const;
    QString getName() const;
    QPointF getPosition() const;
    void setPosition(const QPointF& pos);
    int getRotationAngle() const;
    void rotateClockwise();
    const QVector<Pin*>& getPins() const;

    QString getType() const { return name; }

    // 🌟 متد اصلی شبیه‌سازی که در کلاس گیت‌ها بازنویسی می‌شود
    virtual void updateState() = 0;

    // 🌟 متدهای کمکی جدید برای ساده‌سازی دسترسی به وضعیت پین‌ها در گیت‌ها
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