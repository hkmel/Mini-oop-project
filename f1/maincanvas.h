#ifndef MAINCANVAS_H
#define MAINCANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>

class MainCanvas : public QGraphicsView {
    Q_OBJECT

public:
    explicit MainCanvas(QWidget *parent=nullptr);
    ~MainCanvas();

    void setCanvasSize(const QString &sizeStr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void drawBackground(QPainter *painter,const QRectF &rect) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene;
    int gridSize;
    bool isPanning;
    QPoint lastMousePos;
};

#endif