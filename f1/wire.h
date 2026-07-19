#ifndef WIRE_H
#define WIRE_H

#include <QGraphicsObject>
#include <QVector>
#include <QPointF>
#include <QPainterPath>
#include "pin.h" // 🌟 اضافه شده برای دسترسی به متدهای منطقی پین مثل getState و setState

class Wire : public QGraphicsObject {
    Q_OBJECT
public:
    Wire(Pin* startPin, QGraphicsItem* parent = nullptr);

    void setEndPin(Pin* pin);
    void setTempEndPoint(const QPointF& pos);
    void addWayPoint(const QPointF& pos);
    bool removeLastWayPoint(); // پاک کردن آخرین تکه سیمِ در حال رسم (پروتئوس استایل)

    Pin* getStartPin() const { return startPin; }
    Pin* getEndPin() const { return endPin; }

    QRectF boundingRect() const override;
    QPainterPath shape() const override; // انتخاب راحت و دقیق سیم با کلیک نزدیک آن
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QVector<QPointF> calculateRoute() const;

    // 🌟 تابع قلب تپنده محاسباتی: انتقال ولتاژ/سیگنال از پین خروجی به پین ورودی
    void propagateSignal() {
        if (!startPin || !endPin) return;

        // تشخیص هوشمند اینکه کدام پین خروجی است و کدام ورودی (مهم نیست کاربر چطور سیم را رسم کرده باشد)
        Pin* outputPin = nullptr;
        Pin* inputPin = nullptr;

        if (startPin->getDirection() == PinDirection::Output) {
            outputPin = startPin;
            inputPin = endPin;
        } else if (endPin->getDirection() == PinDirection::Output) {
            outputPin = endPin;
            inputPin = startPin;
        }

        // انتقال سیگنال: پین ورودی وضعیت پین خروجی را به خود می‌گیرد
        if (outputPin && inputPin) {
            inputPin->setState(outputPin->getState());
        }
    }

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    Pin* startPin;
    Pin* endPin;
    QPointF tempEndPoint;
    QVector<QPointF> wayPoints;
    bool isTemp;
    bool isHovered; // بررسی وضعیت قرارگیری موس روی سیم
};

#endif // WIRE_H