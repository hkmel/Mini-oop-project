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
#include <QVector>

class Component;
class Wire;
class Pin;

class MainCanvas : public QGraphicsView {
    Q_OBJECT

signals:
    // هماهنگ شده با MainWindow
    void mouseMoved(const QPointF& pos);
    void componentPlaced();
    void zoomChanged(int zoom);
    void componentSelected(Component* comp);

public:
    explicit MainCanvas(QWidget* parent = nullptr);
    ~MainCanvas();
    void setCanvasSize(const QString& sizeStr);
    QPointF snapToGrid(const QPointF& pos);
    qreal getZoomLevel() const;
    void addComponent(const QString& type, const QPointF& pos);

    // هماهنگ شده با نام متد در MainWindow
    void setCurrentSelectedType(const QString& type);

    void zoomToFit();

    QVector<Wire*> getWires() const { return wires; }
    Pin* findPinAt(const QPointF& scenePos);

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

    QVector<Wire*> wires;
    Wire* activeWire;
    Pin* hoveredPin;

    void updateZoomValue();
};

#endif // MAINCANVAS_H