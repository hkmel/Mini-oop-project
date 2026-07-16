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

public:
    explicit MainCanvas(QWidget* parent = nullptr);
    ~MainCanvas();
    void setCanvasSize(const QString& sizeStr);
    QPointF snapToGrid(const QPointF& pos);
    qreal getZoomLevel() const;
    void addComponent(const QString& type, const QPointF& pos);
    void setActiveComponentType(const QString& type);
    void zoomToFit();

    // متد کمکی برای دسترسی به لیست سیم‌های رسم شده
    QVector<Wire*> getWires() const { return wires; }

    // متد کمکی برای پیدا کردن پین در مختصات خاص موس
    Pin* findPinAt(const QPointF& scenePos);

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

    // متغیرهای جدید برای مدیریت فرآیند سیم‌کشی
    QVector<Wire*> wires;  // لیست کل سیم‌های مدار
    Wire* activeWire;      // سیمی که همین الان کاربر در حال کشیدنش هست
    Pin* hoveredPin;       // پینی که موس الان روش قرار داره (برای هایلایت سبز)

    void updateZoomValue();
};

#endif // MAINCANVAS_H