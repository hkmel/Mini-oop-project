#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QLabel>
#include <QDockWidget>
#include <QToolBar>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include "startmenu.h"
#include "maincanvas.h"
class ComponentPreviewWidget:public QWidget{
    Q_OBJECT
    QString cType;
public:
    ComponentPreviewWidget(QWidget*p=nullptr):QWidget(p){}
    void setComponentType(const QString&t){cType=t;update();}
protected:
    void paintEvent(QPaintEvent*e)override{
        Q_UNUSED(e);
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(),QColor(235,232,215));
        if(cType.isEmpty())return;
        p.translate(width()/2,height()/2);
        p.setPen(QPen(QColor(139,0,0),3));
        p.setBrush(QBrush(QColor(235,232,215)));
        if(cType=="Resistor"){
            p.drawLine(-30,0,-15,0);p.drawRect(-15,-8,30,16);p.drawLine(15,0,30,0);
        }else if(cType=="Capacitor"){
            p.drawLine(-30,0,-6,0);p.drawLine(-6,-15,-6,15);p.drawLine(6,-15,6,15);p.drawLine(6,0,30,0);
        }else if(cType=="Inductor"){
            p.drawLine(-30,0,-20,0);p.drawArc(-20,-8,10,16,0,180*16);p.drawArc(-10,-8,10,16,0,180*16);p.drawArc(0,-8,10,16,0,180*16);p.drawArc(10,-8,10,16,0,180*16);p.drawLine(20,0,30,0);
        }else if(cType=="GND"){
            p.drawLine(0,-15,0,5);p.drawLine(-15,5,15,5);p.drawLine(-10,10,10,10);p.drawLine(-5,15,5,15);
        }else if(cType=="VoltageSource"){
            p.drawEllipse(-15,-15,30,30);p.drawLine(0,-22,0,-15);p.drawLine(0,15,0,22);p.drawText(QRectF(-15,-12,30,12),Qt::AlignCenter,"+");p.drawText(QRectF(-15,0,30,12),Qt::AlignCenter,"-");
        }else if(cType=="Switch"){
            p.drawLine(-30,0,-15,0);p.drawEllipse(-15,-2,4,4);p.drawEllipse(11,-2,4,4);p.drawLine(-13,0,10,-12);p.drawLine(15,0,30,0);
        }else if(cType=="Button"){
            p.drawLine(-30,0,-15,0);p.drawEllipse(-15,-2,4,4);p.drawEllipse(11,-2,4,4);p.drawLine(-13,-6,11,-6);p.drawLine(-1,-6,-1,-12);p.drawLine(-8,-12,6,-12);p.drawLine(15,0,30,0);
        }else if(cType=="AND"){
            p.drawLine(-30,-10,-15,-10);p.drawLine(-30,10,-15,10);QPainterPath path;path.moveTo(-15,-20);path.lineTo(0,-20);path.arcTo(-15,-20,40,40,90,-180);path.lineTo(-15,20);path.closeSubpath();p.drawPath(path);p.drawLine(25,0,30,0);
        }else if(cType=="OR"){
            p.drawLine(-30,-10,-11,-10);p.drawLine(-30,10,-11,10);QPainterPath path;path.moveTo(-15,-20);path.arcTo(-25,-20,20,40,90,-180);path.arcTo(-45,-20,70,40,90,-90);path.arcTo(-45,-20,70,40,0,-90);path.closeSubpath();p.drawPath(path);p.drawLine(25,0,30,0);
        }else if(cType=="NOT"){
            p.drawLine(-30,0,-15,0);QPainterPath path;path.moveTo(-15,-15);path.lineTo(10,0);path.lineTo(-15,15);path.closeSubpath();p.drawPath(path);p.drawEllipse(10,-3,6,6);p.drawLine(16,0,30,0);
        }else if(cType=="NAND"){
            p.drawLine(-30,-10,-15,-10);p.drawLine(-30,10,-15,10);QPainterPath path;path.moveTo(-15,-20);path.lineTo(0,-20);path.arcTo(-15,-20,40,40,90,-180);path.lineTo(-15,20);path.closeSubpath();p.drawPath(path);p.drawEllipse(25,-3,6,6);p.drawLine(31,0,35,0);
        }else if(cType=="XOR"){
            p.drawLine(-30,-10,-13,-10);p.drawLine(-30,10,-13,10);QPainterPath path;path.moveTo(-15,-20);path.arcTo(-25,-20,20,40,90,-180);path.arcTo(-45,-20,70,40,90,-90);path.arcTo(-45,-20,70,40,0,-90);path.closeSubpath();p.drawPath(path);p.drawArc(-29,-20,20,40,90,-180);p.drawLine(25,0,30,0);
        }else if(cType=="D Flip-Flop"){
            p.drawRect(-20,-25,40,50);p.drawLine(-20,-10,-30,-10);p.drawLine(-20,10,-30,10);p.drawLine(20,-10,30,-10);p.drawLine(20,10,30,10);p.drawText(QRectF(-18,-18,15,15),Qt::AlignCenter,"D");p.drawText(QRectF(-18,2,20,15),Qt::AlignCenter,"C");p.drawText(QRectF(2,-18,15,15),Qt::AlignCenter,"Q");p.drawText(QRectF(2,2,18,15),Qt::AlignCenter,"QB");
        }else if(cType=="LED"){
            p.drawLine(-30,0,-10,0);QPainterPath path;path.moveTo(-10,-12);path.lineTo(10,0);path.lineTo(-10,12);path.closeSubpath();p.drawPath(path);p.drawLine(10,-12,10,12);p.drawLine(10,0,30,0);p.drawLine(-3,-12,5,-20);p.drawLine(5,-20,2,-19);p.drawLine(5,-20,4,-17);
        }else if(cType=="7-Segment"){
            p.drawRect(-20,-30,40,60);p.drawText(QRectF(-20,-30,40,60),Qt::AlignCenter,"8");
        }else{
            p.drawText(QRectF(-40,-40,80,80),Qt::AlignCenter,cType);
        }
    }
};
class MainWindow:public QMainWindow{
    Q_OBJECT
public:
    MainWindow(QWidget*parent=nullptr);
    ~MainWindow();
private slots:
    void handleNewProject(const QString&pageSize);
    void handleOpenProject();
    void onMouseMoved(const QPointF&pos);
    void onZoomChanged(int percentage);
    void onComponentSelected(Component*comp);
    void onPropertyValueChanged(QTableWidgetItem*item);
    void filterLibrary(const QString&text);
    void onTreeItemClicked(QTreeWidgetItem*item,int col);
    void onAddActiveClicked();
    void onRemoveActiveClicked();
    void onActiveListClicked(QListWidgetItem*item);
private:
    void createWorkspaceUI();
    QStackedWidget*stackedWidget;
    StartMenu*startMenu;
    MainCanvas*mainCanvas;
    QLineEdit*searchEdit;
    QTreeWidget*libraryTree;
    ComponentPreviewWidget*previewWidget;
    QPushButton*btnAddActive;
    QPushButton*btnRemoveActive;
    QListWidget*activeList;
    QTableWidget*propertiesTable;
    QLabel*coordLabel;
    QLabel*zoomLabel;
    QLabel*helpLabel;
    Component*activeComponent;
    QDockWidget*libraryDock;
    QDockWidget*propertiesDock;
    QToolBar*toolbar;
};
#endif