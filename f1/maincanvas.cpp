#include "maincanvas.h"
#include <QPainter>
#include <QScrollBar>

MainCanvas::MainCanvas(QWidget *parent) : QGraphicsView(parent) {
    scene=new QGraphicsScene(this);
    setScene(scene);

    gridSize=20;
    isPanning=false;

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(QColor("#1e1e1e"));

    setCanvasSize("A4 (Standard)");
}

MainCanvas::~MainCanvas() {}

void MainCanvas::setCanvasSize(const QString &sizeStr) {
    if (sizeStr.contains("A3")) {
        scene->setSceneRect(0,0,1600,1200);
    } else if (sizeStr.contains("Custom")) {
        scene->setSceneRect(0,0,1000,1000);
    } else {
        scene->setSceneRect(0,0,1200,900);
    }
}

void MainCanvas::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsView::drawBackground(painter,rect);

    painter->setPen(QPen(QColor("#2d2d2d"),1,Qt::SolidLine));

    qreal left=int(rect.left())-(int(rect.left()) % gridSize);
    qreal top=int(rect.top())-(int(rect.top()) % gridSize);

    for (qreal x=left;x<rect.right();x += gridSize) {
        painter->drawLine(x,rect.top(),x,rect.bottom());
    }
    for (qreal y=top;y<rect.bottom(); y += gridSize) {
        painter->drawLine(rect.left(),y,rect.right(), y);
    }
}

void MainCanvas::wheelEvent(QWheelEvent *event) {
    qreal scaleFactor=1.15;
    if (event->angleDelta().y()>0) {
        scale(scaleFactor,scaleFactor);
    } else {
        scale(1.0/scaleFactor,1.0/scaleFactor);
    }
}

void MainCanvas::mousePressEvent(QMouseEvent *event) {
    if (event->button()==Qt::MiddleButton) {
        isPanning=true;
        lastMousePos=event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void MainCanvas::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button()==Qt::MiddleButton) {
        isPanning=false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MainCanvas::mouseMoveEvent(QMouseEvent *event) {
    if (isPanning) {
        QPointF delta=mapToScene(lastMousePos) - mapToScene(event->pos());
        lastMousePos=event->pos();

        horizontalScrollBar()->setValue(horizontalScrollBar()->value()+delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value()+delta.y());
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}