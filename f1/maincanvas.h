#ifndef MAINCANVAS_H

#define MAINCANVAS_H



#include <QGraphicsView>

#include <QGraphicsScene>

#include <QWheelEvent>

#include <QMouseEvent>

#include <QKeyEvent>

#include <QPointF>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QString>

#include <QRubberBand>

#include <QVector>

#include <QResizeEvent>

#include <QTimer>

#include <QLabel>



class Component;

class Wire;

class Pin;



class MainCanvas : public QGraphicsView {

    Q_OBJECT



signals:

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

    void setCurrentSelectedType(const QString& type);

    void zoomToFit();

    QVector<Wire*> getWires() const { return wires; }

    Pin* findPinAt(const QPointF& scenePos);

    void startSimulation();

    void pauseSimulation();

    void stopSimulation();

    bool getIsSimulating() const { return isSimulating; }
    bool saveToFile(const QString &filePath);
    bool loadFromFile(const QString &filePath);
    void clearCanvas();



protected:

    void wheelEvent(QWheelEvent* event) override;

    void drawBackground(QPainter* painter, const QRectF& rect) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;



private slots:

    void runSimulationStep();

    void updateClock();



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

    QWidget* floatingControlPanel;

    QLabel* lblSimStatus;

    QLabel* lblTimerDisplay;

    void createFloatingControlPanel();

    void updateTimerLabel();

    QVector<Wire*> wires;

    Wire* activeWire;

    Pin* hoveredPin;

    QTimer* simulationTimer;

    QTimer* clockTimer;

    int simElapsedTenths;

    bool isSimulating;

    void updateZoomValue();

};



#endif // MAINCANVAS_H