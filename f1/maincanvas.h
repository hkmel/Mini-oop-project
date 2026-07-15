#ifndef MAINCANVAS_H
#define MAINCANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPointF>
#include <QString>
#include <QRubberBand>

class Component;

class MainCanvas : public QGraphicsView {
    Q_OBJECT

public:
    explicit MainCanvas(QWidget* parent=nullptr);
    ~MainCanvas();
    void setCanvasSize(const QString& sizeStr);
    QPointF snapToGrid(const QPointF& pos);
    qreal getZoomLevel() const;
    void addComponent(const QString& type, const QPointF& pos);
    void setActiveComponentType(const QString& type);
    void zoomToFit();

signals:
    void mousePositionChanged(const QPointF& pos);
    void zoomChanged(int zoom);
    void componentSelected(Component* comp);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QGraphicsScene* scene;
    int gridSize;
    bool isPanning;
    bool leftDragging;
    QPoint lastMousePos;
    qreal currentZoom;
    QString activeComponentType;
    QRubberBand* rubberBand;
    QPoint rubberOrigin;

    void updateZoomValue();
};

#endif