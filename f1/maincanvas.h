#ifndef MAINCANVAS_H
#define MAINCANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QPointF>
#include <QVector>
#include <QString>

class Component;
class Wire;
class Pin;

class MainCanvas : public QGraphicsView {
    Q_OBJECT

public:
    MainCanvas(QWidget* parent = nullptr);
    ~MainCanvas();

    void setCanvasSize(const QString& sizeStr);
    void setCurrentSelectedType(const QString& type);
    void setActiveComponentType(const QString& type);
    void addComponent(const QString& type, const QPointF& pos);
    void zoomToFit();

    qreal getZoomLevel() const;
    Pin* findPinAt(const QPointF& scenePos);
    QVector<Wire*>& getWires() { return wires; }

signals:
    void mouseMoved(const QPointF& pos);
    void zoomChanged(int percentage);
    void componentSelected(Component* comp);
    void componentPlaced();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    QPointF snapToGrid(const QPointF& pos);
    void updateZoomValue();
    void updateConnectedWiresForSelected(); // متد کمکی برای آپدیت زنده سیم‌های قطعات در حال جابه‌جایی

    QGraphicsScene* scene;
    int gridSize;
    qreal currentZoom;
    QString activeComponentType;

    bool isPanning;
    QPoint lastMousePos;

    Wire* activeWire;
    Pin* hoveredPin;
    QVector<Wire*> wires;
};

#endif // MAINCANVAS_H