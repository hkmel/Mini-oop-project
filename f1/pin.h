#ifndef PIN_H
#define PIN_H

#include <QString>
#include <QPointF>

class Component;

class Pin {
public:
    Pin(const QString& id, const QPointF& offset, Component* parent)
        : id(id), offset(offset), parentComponent(parent) {}

    QString getId() const { return id; }
    QPointF getOffset() const { return offset; }
    Component* getParentComponent() const { return parentComponent; }

    // محاسبه مختصات دقیق پین روی بوم با توجه به موقعیت و چرخش کامپوننت
    QPointF getGlobalPosition() const;

private:
    QString id;
    QPointF offset; // موقعیت نسبت به مرکز قطعه (مثلاً -30, 0)
    Component* parentComponent;
};

#endif // PIN_H