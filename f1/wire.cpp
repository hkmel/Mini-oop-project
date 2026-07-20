#include "wire.h"
#include "pin.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPainterPathStroker>

Wire::Wire(Pin* startPin, QGraphicsItem* parent)
    : QGraphicsObject(parent), startPin(startPin), endPin(nullptr), isTemp(true), isHovered(false) {
    setZValue(-1);
    setFlag(QGraphicsItem::ItemIsSelectable, true); // فعال کردن قابلیت کلیک و انتخاب سیم
    setAcceptHoverEvents(true); // فعال کردن حساسیت موس به هاور شدن روی سیم
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

bool Wire::removeLastWayPoint() {
    if (!wayPoints.isEmpty()) {
        wayPoints.removeLast();
        update();
        return true; // نقطه شکست قبلی با موفقیت لغو شد
    }
    return false; // هیچ نقطه‌ای برای لغو وجود ندارد
}

QVector<QPointF> Wire::calculateRoute() const {
    QVector<QPointF> route;
    if (!startPin) return route;

    route.append(startPin->getGlobalPosition());

    for (const QPointF& wp : wayPoints) {
        QPointF pPrev = route.last();
        if (pPrev.x() != wp.x() && pPrev.y() != wp.y()) {
            route.append(QPointF(wp.x(), pPrev.y()));
        }
        route.append(wp);
    }

    QPointF pLast = route.last();
    QPointF pEnd = (isTemp) ? tempEndPoint : (endPin ? endPin->getGlobalPosition() : pLast);

    if (pLast.x() != pEnd.x() && pLast.y() != pEnd.y()) {
        route.append(QPointF(pEnd.x(), pLast.y()));
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

// ساخت حریم کلیک مجازی به ضخامت ۱۰ پیکسل به موازات سیم
QPainterPath Wire::shape() const {
    QPainterPath path;
    QVector<QPointF> pts = calculateRoute();
    if (pts.size() < 2) return path;

    path.moveTo(pts[0]);
    for (int i = 1; i < pts.size(); ++i) {
        path.lineTo(pts[i]);
    }

    QPainterPathStroker stroker;
    stroker.setWidth(10); // کاربر تا فاصله ۵ پیکسلی سیم هم کلیک کند، سیم انتخاب می‌شود
    return stroker.createStroke(path);
}

void Wire::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option); Q_UNUSED(widget);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // ۱. تشخیص وضعیت ولتاژ سیم بر اساس پین‌ها
    PinState state = PinState::Floating;
    if (startPin) state = startPin->getState();
    if (state == PinState::Floating && endPin) state = endPin->getState();

    // ۲. اعمال رنگ‌بندی داینامیک نئونی (پروتئوس استایل)
    QPen pen;
    if (state == PinState::High) {
        pen = QPen(QColor(255, 45, 85), 3); // قرمز نئونی برای ۵ ولت (HIGH)
    } else if (state == PinState::Low) {
        pen = QPen(QColor(0, 210, 255), 2.5); // آبی نئونی برای ۰ ولت (LOW)
    } else {
        pen = QPen(QColor(120, 120, 120), 2, Qt::DashLine); // خاکستری دش‌پوینت برای Floating
    }

    if (isHovered || isSelected()) {
        pen.setWidthF(pen.widthF() + 1.5);
        pen.setColor(QColor(255, 255, 0)); // زرد موقع هاور یا انتخاب
    }

    painter->setPen(pen);

    // ۳. رسم مسیر سیم
    QVector<QPointF> route = calculateRoute();
    for (int i = 0; i < route.size() - 1; ++i) {
        painter->drawLine(route[i], route[i + 1]);
    }

    painter->restore();
}

void Wire::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void Wire::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = false;
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}