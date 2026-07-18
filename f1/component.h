#ifndef COMPONENT_H
#define COMPONENT_H

#include <QString>
#include <QPointF>
#include <QVector>

class Pin;

class Component {
public:
    Component(const QString& id, const QString& name, const QPointF& pos);

    // ۱. مخرب مجازی برای پاک کردن پین‌ها از حافظه و جلوگیری از Memory Leak
    virtual ~Component();

    QString getId() const;
    QString getName() const;
    QPointF getPosition() const;
    void setPosition(const QPointF& pos);
    int getRotationAngle() const;
    void rotateClockwise();
    const QVector<Pin*>& getPins() const;

    // ۲. اضافه شدن متد تعیین نوع قطعه برای هماهنگی با پنل قطعات فعال و بوم
    QString getType() const { return name; }

    virtual void updateState() = 0;

protected:
    void addPin(Pin* pin);
    QString id;
    QString name; // نام یا نوع اصلی قطعه (مثلاً Resistor)
    QPointF position;
    int rotationAngle;
    QVector<Pin*> pins;
};

#endif // COMPONENT_H