#ifndef COMPONENTLIBRARY_H
#define COMPONENTLIBRARY_H
#include<QString>
#include<QStringList>
#include<QMap>
#include<QPointF>
class Component;
class ComponentLibrary{
public:
    static ComponentLibrary&getInstance();
    QStringList getCategories()const;
    QStringList getComponentsInCategory(const QString&category)const;
    Component*createComponent(const QString&type,const QString&id,const QPointF&pos);
private:
    ComponentLibrary();
    ~ComponentLibrary();
    QMap<QString,QStringList>libraryData;
};
#endif