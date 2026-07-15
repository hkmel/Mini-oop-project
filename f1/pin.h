#ifndef PIN_H
#define PIN_H
#include <QString>
#include <QPointF>
class Component;
class Pin{
public:
    Pin(const QString& id,const QPointF& offset,Component* parent);
    ~Pin();
    QString getId() const;
    QPointF getOffset() const;
    QPointF getAbsolutePosition() const;
    Component* getParentComponent() const;
    void setConnected(bool connected);
    bool isConnected() const;
private:
    QString id;
    QPointF offset;
    Component* parentComponent;
    bool connectedState;
};
#endif