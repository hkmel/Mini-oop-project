#ifndef PIN_H
#define PIN_H

#include <QString>
#include <QPointF>

class Component;

// 🌟 اضافه شدن امپدانس‌ها و وضعیت‌های منطقی استاندارد دیجیتال
enum class PinState {
    Low = 0,
    High = 1,
    Floating = 2
};

// 🌟 تعیین جهت پین برای تشخیص مسیر حرکت سیگنال‌ها
enum class PinDirection {
    Input,
    Output
};

class Pin {
public:
    Pin(const QString& id, const QPointF& offset, Component* parent, PinDirection dir = PinDirection::Input)
        : id(id), offset(offset), parentComponent(parent), direction(dir), state(PinState::Floating) {}

    QString getId() const { return id; }
    QPointF getOffset() const { return offset; }
    Component* getParentComponent() const { return parentComponent; }

    // 🌟 متدهای جدید برای دسترسی و تغییر وضعیت منطقی پین
    PinDirection getDirection() const { return direction; }
    PinState getState() const { return state; }
    void setState(PinState newState) { state = newState; }

    // محاسبه مختصات دقیق پین روی بوم با توجه به موقعیت و چرخش کامپوننت
    QPointF getGlobalPosition() const;

private:
    QString id;
    QPointF offset;
    Component* parentComponent;

    // 🌟 متغیرهای جدید منطق دیجیتال
    PinDirection direction;
    PinState state;
};

#endif // PIN_H