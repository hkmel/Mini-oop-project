#ifndef WIRE_H
#define WIRE_H

#include <QGraphicsObject>
#include <QVector>
#include <QPointF>

class Pin;

class Wire : public QGraphicsObject {
    Q_OBJECT
public:
    Wire(Pin* startPin, QGraphicsItem* parent = nullptr);

    void setEndPin(Pin* pin);
    void setTempEndPoint(const QPointF& pos);
    void addWayPoint(const QPointF& pos); // اضافه کردن نقطه شکست دلخواه با کلیک روی بوم

    Pin* getStartPin() const { return startPin; }
    Pin* getEndPin() const { return endPin; }

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    // محاسبه تمام نقاط مسیر (شامل نقاط میانی و انتهای موقت) به صورت ۹۰ درجه
    QVector<QPointF> calculateRoute() const;

private:
    Pin* startPin;
    Pin* endPin;
    QPointF tempEndPoint; // موقعیت فعلی موس
    QVector<QPointF> wayPoints; // نقاط شکستگی که کاربر با کلیک ثبت کرده است
    bool isTemp;
};

#endif // WIRE_H