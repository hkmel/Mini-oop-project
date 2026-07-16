#include "wire.h"
#include "pin.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

Wire::Wire(Pin* startPin, QGraphicsItem* parent)
    : QGraphicsObject(parent), startPin(startPin), endPin(nullptr), isTemp(true) {
    setZValue(-1); // سیم‌ها پشت قطعات رسم شوند
}

void Wire::setEndPin(Pin* pin) {
    endPin = pin;
    isTemp = false;
    update();
}

void Wire::setTempEndPoint(const QPointF& pos) {
    tempEndPoint = pos;
    update();
}

void Wire::addWayPoint(const QPointF& pos) {
    wayPoints.append(pos);
    update();
}

QVector<QPointF> Wire::calculateRoute() const {
    QVector<QPointF> route;
    if (!startPin) return route;

    // ۱. اضافه کردن نقطه شروع (پین مبدا)
    route.append(startPin->getGlobalPosition());

    // ۲. اضافه کردن نقاط شکست واسط که کاربر قبلاً کلیک کرده است
    for (const QPointF& wp : wayPoints) {
        QPointF pPrev = route.last();
        // ایجاد اتصال ۹۰ درجه بین نقطه قبلی و نقطه شکست جدید
        if (pPrev.x() != wp.x() && pPrev.y() != wp.y()) {
            route.append(QPointF(wp.x(), pPrev.y())); // اول افقی، بعد عمودی
        }
        route.append(wp);
    }

    // ۳. انتهای مسیر (یا پین مقصد یا موقعیت موقت موس)
    QPointF pLast = route.last();
    QPointF pEnd = (isTemp) ? tempEndPoint : (endPin ? endPin->getGlobalPosition() : pLast);

    if (pLast.x() != pEnd.x() && pLast.y() != pEnd.y()) {
        route.append(QPointF(pEnd.x(), pLast.y())); // اتصال ۹۰ درجه تا نقطه نهایی
    }
    route.append(pEnd);

    return route;
}

QRectF Wire::boundingRect() const {
    QVector<QPointF> pts = calculateRoute();
    if (pts.isEmpty()) return QRectF();

    qreal minX = pts[0].x(), maxX = pts[0].x();
    qreal minY = pts[0].y(), maxY = pts[0].y();

    for (const QPointF& p : pts) {
        minX = qMin(minX, p.x());
        maxX = qMax(maxX, p.x());
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    }
    return QRectF(minX - 5, minY - 5, (maxX - minX) + 10, (maxY - minY) + 10);
}

void Wire::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option); Q_UNUSED(widget);
    QVector<QPointF> pts = calculateRoute();
    if (pts.size() < 2) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    if (isTemp) {
        painter->setPen(QPen(Qt::cyan, 2, Qt::DashLine)); // سیم موقت در حال رسم
    } else {
        painter->setPen(QPen(QColor(0, 180, 255), 2)); // سیم قطعی آبی‌رنگ
    }

    for (int i = 0; i < pts.size() - 1; ++i) {
        painter->drawLine(pts[i], pts[i+1]);
    }
    painter->restore();
}