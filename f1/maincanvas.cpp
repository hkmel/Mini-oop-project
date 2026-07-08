#include "maincanvas.h"

#include <QPainter>
#include <QScrollBar>
#include <QtMath>



MainCanvas::MainCanvas(QWidget *parent)
    : QGraphicsView(parent)
{

    scene = new QGraphicsScene(this);


    setScene(scene);



    gridSize = 20;


    isPanning = false;


    currentZoom = 1.0;



    setRenderHint(QPainter::Antialiasing);

    setRenderHint(QPainter::SmoothPixmapTransform);



    setTransformationAnchor(
        QGraphicsView::AnchorUnderMouse
        );


    setResizeAnchor(
        QGraphicsView::AnchorUnderMouse
        );



    setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );


    setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );



    setMouseTracking(true);



    setBackgroundBrush(
        QColor("#151515")
        );



    setCanvasSize("A4");

}



MainCanvas::~MainCanvas()
{

}




void MainCanvas::setCanvasSize(const QString &sizeStr)
{

    if(sizeStr.contains("A3"))
    {

        scene->setSceneRect(
            0,
            0,
            1600,
            1200
            );

    }

    else if(sizeStr.contains("Custom"))
    {

        scene->setSceneRect(
            0,
            0,
            1000,
            1000
            );

    }

    else
    {

        scene->setSceneRect(
            0,
            0,
            1200,
            900
            );

    }


}




QPointF MainCanvas::snapToGrid(const QPointF &pos)
{

    qreal x =
        qRound(pos.x()/gridSize)
        * gridSize;


    qreal y =
        qRound(pos.y()/gridSize)
        * gridSize;



    return QPointF(x,y);

}





void MainCanvas::drawBackground(
    QPainter *painter,
    const QRectF &rect)
{


    QGraphicsView::drawBackground(
        painter,
        rect
        );



    painter->save();



    int smallGrid = gridSize;


    int bigGrid = gridSize*5;



    painter->setPen(
        QPen(
            QColor("#303030"),
            1
            )
        );


    qreal left =
        int(rect.left())
        -
        (int(rect.left())%smallGrid);



    qreal top =
        int(rect.top())
        -
        (int(rect.top())%smallGrid);



    for(qreal x=left;
         x<rect.right();
         x+=smallGrid)
    {

        painter->drawLine(
            x,
            rect.top(),
            x,
            rect.bottom()
            );

    }



    for(qreal y=top;
         y<rect.bottom();
         y+=smallGrid)
    {

        painter->drawLine(
            rect.left(),
            y,
            rect.right(),
            y
            );

    }





    painter->setPen(
        QPen(
            QColor("#505050"),
            1.5
            )
        );



    left =
        int(rect.left())
        -
        (int(rect.left())%bigGrid);



    top =
        int(rect.top())
        -
        (int(rect.top())%bigGrid);



    for(qreal x=left;
         x<rect.right();
         x+=bigGrid)
    {

        painter->drawLine(
            x,
            rect.top(),
            x,
            rect.bottom()
            );

    }



    for(qreal y=top;
         y<rect.bottom();
         y+=bigGrid)
    {

        painter->drawLine(
            rect.left(),
            y,
            rect.right(),
            y
            );

    }



    // Origin

    painter->setPen(
        QPen(
            QColor("#00ff88"),
            2
            )
        );


    painter->drawLine(
        0,
        rect.top(),
        0,
        rect.bottom()
        );


    painter->drawLine(
        rect.left(),
        0,
        rect.right(),
        0
        );



    painter->restore();


}






void MainCanvas::wheelEvent(QWheelEvent *event)
{

    double factor = 1.15;


    if(event->angleDelta().y()>0)
    {

        if(currentZoom < 3)
        {

            scale(
                factor,
                factor
                );


            currentZoom*=factor;

        }

    }

    else
    {

        if(currentZoom >0.3)
        {

            scale(
                1/factor,
                1/factor
                );


            currentZoom/=factor;

        }

    }


    updateZoomValue();

}





void MainCanvas::mousePressEvent(QMouseEvent *event)
{

    if(event->button()==Qt::MiddleButton ||
        event->button()==Qt::LeftButton)
    {

        isPanning=true;

        lastMousePos=event->pos();

        setCursor(Qt::ClosedHandCursor);

        event->accept();

        return;
    }


    QGraphicsView::mousePressEvent(event);

}





void MainCanvas::mouseReleaseEvent(QMouseEvent *event)
{

    if(event->button()==Qt::MiddleButton)
    {


        isPanning=false;
        leftDragging=false;


        setCursor(
            Qt::ArrowCursor
            );


        event->accept();


        return;

    }


    QGraphicsView::mouseReleaseEvent(event);

}





void MainCanvas::mouseMoveEvent(QMouseEvent *event)
{


    QPointF scenePos =
        mapToScene(event->pos());



    emit mousePositionChanged(
        scenePos
        );



    if(isPanning)
    {

        QPointF delta =
            mapToScene(lastMousePos)
            -
            mapToScene(event->pos());



        lastMousePos =
            event->pos();



        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value()
            +
            delta.x()
            );


        verticalScrollBar()->setValue(
            verticalScrollBar()->value()
            +
            delta.y()
            );


        event->accept();


        return;

    }



    QGraphicsView::mouseMoveEvent(event);

}





void MainCanvas::keyPressEvent(QKeyEvent *event)
{


    if(event->key()==Qt::Key_R)
    {

        resetTransform();


        currentZoom=1.0;


        updateZoomValue();

        return;

    }



    QGraphicsView::keyPressEvent(event);

}





qreal MainCanvas::getZoomLevel() const
{

    return currentZoom;

}





void MainCanvas::updateZoomValue()
{

    emit zoomChanged(
        int(currentZoom*100)
        );

}