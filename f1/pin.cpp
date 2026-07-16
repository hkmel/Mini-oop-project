#include "pin.h"
#include "component.h"
#include <QtMath>

QPointF Pin::getGlobalPosition() const {
    if (!parentComponent) return QPointF(0, 0);

    QPointF compPos = parentComponent->getPosition();
    int angle = parentComponent->getRotationAngle();
    qreal rad = qDegreesToRadians(qreal(angle));

    // اعمال ماتریس دوران حول مرکز قطعه
    qreal rx = offset.x() * qCos(rad) - offset.y() * qSin(rad);
    qreal ry = offset.x() * qSin(rad) + offset.y() * qCos(rad);

    return compPos + QPointF(rx, ry);
}