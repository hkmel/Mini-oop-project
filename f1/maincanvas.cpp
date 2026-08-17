#include "maincanvas.h"

#include "componentlibrary.h"

#include "basecomponents.h"

#include "pin.h"

#include "wire.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFrame>
#include "probe.h"

#include <QGraphicsItem>
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPainter>

#include <QScrollBar>

#include <QtMath>

#include <QMenu>

#include <QGraphicsSceneMouseEvent>

#include <QInputDialog>

#include <QMessageBox>
#include <QFileDialog>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>
#include <QHBoxLayout>

#include <QVBoxLayout>

#include <QPushButton>

#include <QLabel>

#include <QLineEdit>

#include <QDialogButtonBox>

#include <QDialog>

#include <QResizeEvent>

#include <QDir>



// کلاس فرعی برای نمایش گرافیکی قطعات روی بوم

class QGraphicsComponentItem : public QGraphicsItem {

public:

    Component* comp;

    QGraphicsComponentItem(Component* c) {
        comp = c;
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        setPos(c->getPosition());

        // 🌟 ۱. فعال‌سازی رویدادهای حرکت موس روی این قطعه
        setAcceptHoverEvents(true);
    }

    QRectF boundingRect() const override {
        return QRectF(-45, -45, 90, 90);
    }




    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {

        Q_UNUSED(option); Q_UNUSED(widget);

        if (isSelected()) {

            painter->save();

            painter->setPen(QPen(Qt::green, 1, Qt::DashLine));

            painter->drawRect(-35, -35, 70, 70);

            painter->restore();

        }

        painter->save();

        int angle = comp->getRotationAngle();

        painter->rotate(angle);

        painter->setPen(QPen(Qt::white, 2));

        painter->setBrush(QBrush(QColor::fromRgb(35, 35, 35)));

        QString name = comp->getName();



        if (name == "Resistor") {
            painter->drawLine(-30, 0, -10, 0);
            painter->drawRect(-10, -5, 20, 10);
            painter->drawLine(10, 0, 30, 0);
        } else if (name == "Capacitor") {
            painter->drawLine(-30, 0, -4, 0);
            painter->drawLine(-4, -12, -4, 12);
            painter->drawLine(4, -12, 4, 12);
            painter->drawLine(4, 0, 30, 0);
        } else if (name == "Inductor") {
            painter->drawLine(-30, 0, -15, 0);
            painter->drawArc(-15, -6, 10, 12, 0, 180 * 16);
            painter->drawArc(-5, -6, 10, 12, 0, 180 * 16);
            painter->drawArc(5, -6, 10, 12, 0, 180 * 16);
            painter->drawLine(15, 0, 30, 0);
        } else if (name == "GND") {
            painter->drawLine(0, -10, 0, 5);
            painter->drawLine(-12, 5, 12, 5);
            painter->drawLine(-8, 10, 8, 10);
            painter->drawLine(-4, 15, 4, 15);
        } else if (name == "VoltageSource") {
            painter->drawEllipse(-12, -12, 24, 24);
            painter->drawLine(0, -20, 0, -12);
            painter->drawLine(0, 12, 0, 20);
            painter->drawText(QRectF(-12, -10, 24, 12), Qt::AlignCenter, "+");
            painter->drawText(QRectF(-12, 0, 24, 12), Qt::AlignCenter, "-");
        } else if (name == "Switch") {
            painter->drawLine(-30, 0, -10, 0);
            painter->drawEllipse(-10, -2, 4, 4);
            painter->drawEllipse(6, -2, 4, 4);
            SwitchComponent* sw = dynamic_cast<SwitchComponent*>(comp);
            if (sw && sw->isOpen()) painter->drawLine(-8, 0, 5, -10);
            else painter->drawLine(-8, 0, 6, 0);
            painter->drawLine(10, 0, 30, 0);
        } else if (name == "Button") {
            painter->drawLine(-30, 0, -10, 0);
            painter->drawEllipse(-10, -2, 4, 4);
            painter->drawEllipse(6, -2, 4, 4);
            ButtonComponent* btn = dynamic_cast<ButtonComponent*>(comp);
            if (btn && btn->isPressed()) painter->drawLine(-8, 0, 6, 0);
            else painter->drawLine(-8, -6, 6, -6);
            painter->drawLine(-1, -6, -1, -12);
            painter->drawLine(-6, -12, 4, -12);
            painter->drawLine(10, 0, 30, 0);
        } else if (name == "AND") {
            painter->drawLine(-30, -10, -10, -10);
            painter->drawLine(-30, 10, -10, 10);
            QPainterPath path;
            path.moveTo(-10, -15);
            path.lineTo(0, -15);
            path.arcTo(-15, -15, 30, 30, 90, -180);
            path.lineTo(-10, 15);
            path.closeSubpath();
            painter->drawPath(path);
            painter->drawLine(15, 0, 30, 0);
        } else if (name == "OR") {
            painter->drawLine(-30, -10, -10, -10);
            painter->drawLine(-30, 10, -10, 10);
            QPainterPath path;
            path.moveTo(-10, -15);
            path.quadTo(0, -15, 10, 0);
            path.quadTo(0, 15, -10, 15);
            path.quadTo(-5, 0, -10, -15);
            painter->drawPath(path);
            painter->drawLine(10, 0, 30, 0);
        } else if (name == "NOT") {
            painter->drawLine(-30, 0, -10, 0);
            QPolygon polygon;
            polygon << QPoint(-10, -10) << QPoint(-10, 10) << QPoint(4, 0);
            painter->drawPolygon(polygon);
            painter->drawEllipse(4, -3, 6, 6);
            painter->drawLine(10, 0, 30, 0);
        } else if (name == "NAND") {
            painter->drawLine(-30, -10, -8, -10);
            painter->drawLine(-30, 10, -8, 10);
            QPainterPath path;
            path.moveTo(-8, -10);
            path.lineTo(0, -10);
            path.arcTo(-10, -10, 20, 20, 90, -180);
            path.lineTo(-8, 10);
            path.closeSubpath();
            painter->drawPath(path);
            painter->drawEllipse(10, -3, 6, 6);
            painter->drawLine(16, 0, 30, 0);
        } else if (name == "XOR") {
            painter->drawLine(-30, -10, -12, -10);
            painter->drawLine(-30, 10, -12, 10);
            QPainterPath backCurve;
            backCurve.moveTo(-14, -15);
            backCurve.quadTo(-8, 0, -14, 15);
            painter->drawPath(backCurve);
            QPainterPath path;
            path.moveTo(-10, -15);
            path.quadTo(0, -15, 10, 0);
            path.quadTo(0, 15, -10, 15);
            path.quadTo(-4, 0, -10, -15);
            painter->drawPath(path);
            painter->drawLine(10, 0, 30, 0);
        } else if (name == "D Flip-Flop") {
            painter->drawRect(-12, -18, 24, 36);
            painter->drawLine(-30, -10, -12, -10);
            painter->drawLine(-30, 10, -12, 10);
            painter->drawLine(12, -10, 30, -10);
            painter->drawLine(12, 10, 30, 10);
            painter->drawLine(-12, 6, -8, 10);
            painter->drawLine(-8, 10, -12, 14);
        } else if (name == "LED") {
            painter->drawLine(-30, 0, -8, 0);
            QPolygon poly;
            poly << QPoint(-8, -8) << QPoint(-8, 8) << QPoint(6, 0);
            painter->drawPolygon(poly);
            painter->drawLine(6, -8, 6, 8);
            painter->drawLine(6, 0, 30, 0);
            painter->drawLine(0, -10, 8, -18);
            painter->drawLine(8, -18, 4, -18);
            painter->drawLine(8, -18, 8, -14);
            painter->drawLine(6, -4, 14, -12);
            painter->drawLine(14, -12, 10, -12);
            painter->drawLine(14, -12, 14, -8);
        } else if (name == "7-Segment") {
            painter->drawRect(-12, -26, 24, 52);
            painter->setPen(QPen(QColor(255, 60, 60, 100), 3, Qt::SolidLine, Qt::RoundCap));
            painter->drawLine(-6, -18, 6, -18);
            painter->drawLine(8, -16, 8, -2);
            painter->drawLine(8, 2, 8, 16);
            painter->drawLine(-6, 18, 6, 18);
            painter->drawLine(-8, 2, -8, 16);
            painter->drawLine(-8, -16, -8, -2);
            painter->drawLine(-6, 0, 6, 0);
            painter->drawPoint(10, 20);
        } else if (name == "VoltageProbe") {
            VoltageProbe* probe = dynamic_cast<VoltageProbe*>(comp);
            double v = probe ? probe->getMeasuredVoltage() : 0.0;
            PinState st = probe ? probe->getMeasuredState() : PinState::Floating;
            painter->setPen(QPen(QColor(0, 255, 136), 1.5));
            painter->setBrush(QBrush(QColor(15, 23, 36, 230)));
            painter->drawRoundedRect(-35, -20, 70, 40, 6, 6);
            painter->setPen(QPen(Qt::white, 2));
            painter->drawLine(-35, 0, -25, 0);
            QFont font("Consolas", 10, QFont::Bold);
            painter->setFont(font);
            if (st == PinState::High) {
                painter->setPen(QColor(255, 45, 85));
                painter->drawText(QRectF(-35, -18, 70, 20), Qt::AlignCenter, QString::number(v, 'f', 1) + " V");
                painter->setFont(QFont("Segoe UI", 7));
                painter->drawText(QRectF(-35, 2, 70, 15), Qt::AlignCenter, "[ HIGH ]");
            } else if (st == PinState::Low) {
                painter->setPen(QColor(0, 210, 255));
                painter->drawText(QRectF(-35, -18, 70, 20), Qt::AlignCenter, QString::number(v, 'f', 1) + " V");
                painter->setFont(QFont("Segoe UI", 7));
                painter->drawText(QRectF(-35, 2, 70, 15), Qt::AlignCenter, "[ LOW ]");
            } else {
                painter->setPen(QColor(241, 196, 15));
                painter->drawText(QRectF(-35, -18, 70, 20), Qt::AlignCenter, "?.? V");
                painter->setFont(QFont("Segoe UI", 7));
                painter->drawText(QRectF(-35, 2, 70, 15), Qt::AlignCenter, "[ FLOAT ]");
            }
        } else {
            painter->drawRect(-30, -30, 60, 60);
            painter->drawText(QRectF(-30, -25, 60, 20), Qt::AlignCenter, comp->getName());
        }

        painter->restore();



        painter->setPen(Qt::yellow);

        painter->setFont(QFont("Arial", 8, QFont::Bold));

        painter->drawText(QRectF(-40, -45, 80, 15), Qt::AlignCenter, comp->getId());



        MainCanvas* canvas = dynamic_cast<MainCanvas*>(scene()->views().first());

        for (Pin* pin : comp->getPins()) {

            QPointF p = pin->getOffset();

            double rx = p.x();

            double ry = p.y();

            if (angle == 90) { rx = -p.y(); ry = p.x(); }

            else if (angle == 180) { rx = -p.x(); ry = -p.y(); }

            else if (angle == 270) { rx = p.y(); ry = -p.x(); }

            QPointF rp(rx, ry);



            if (canvas && canvas->findPinAt(canvas->mapToScene(canvas->mapFromScene(pin->getGlobalPosition()))) == pin) {

                painter->setPen(QPen(Qt::green, 7));

            } else {

                painter->setPen(QPen(Qt::red, 5));

            }

            painter->drawPoint(rp);



            painter->setPen(QPen(Qt::cyan, 1));

            painter->setFont(QFont("Arial", 7));

            if (rp.x() < 0) painter->drawText(rp + QPointF(5, 3), pin->getId());

            else painter->drawText(rp + QPointF(-25, 3), pin->getId());

        }

    }



protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override {

        if (change == ItemPositionChange && scene()) {

            QPointF newPos = value.toPointF();

            qreal x = qRound(newPos.x() / 20.0) * 20.0;

            qreal y = qRound(newPos.y() / 20.0) * 20.0;

            comp->setPosition(QPointF(x, y));



            MainCanvas* canvas = dynamic_cast<MainCanvas*>(scene()->views().first());

            if (canvas) {

                for (Wire* wire : canvas->getWires()) {

                    if (wire->getStartPin()->getParentComponent() == comp ||

                        (wire->getEndPin() && wire->getEndPin()->getParentComponent() == comp)) {

                        wire->update();

                    }

                }

            }

            return QPointF(x, y);

        }

        return QGraphicsItem::itemChange(change, value);

    }
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override {
        QPointF mousePos = event->pos(); // موقعیت موس نسبت به مرکز همین قطعه
        bool pinHovered = false;

        // زاویه چرخش قطعه
        int angle = comp->getRotationAngle();

        for (Pin* pin : comp->getPins()) {
            QPointF p = pin->getOffset();
            double rx = p.x();
            double ry = p.y();

            // اعمال چرخش روی مختصات offset پین
            if (angle == 90) { rx = -p.y(); ry = p.x(); }
            else if (angle == 180) { rx = -p.x(); ry = -p.y(); }
            else if (angle == 270) { rx = p.y(); ry = -p.x(); }
            QPointF pinPos(rx, ry);

            // محاسبه فاصله موس تا پین (مثلاً ۱۰ پیکسل برای تشخیص شعاع نزدیک شدن)
            qreal distance = QLineF(mousePos, pinPos).length();

            if (distance <= 10.0) { // اگر موس کمتر از ۱۰ پیکسل با پین فاصله داشت
                pinHovered = true;
                break;
            }
        }

        // درخواست رسم مجدد قطعه برای به‌روزرسانی رنگ پین در paint()
        update();

        QGraphicsItem::hoverMoveEvent(event);
    }

    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override {
        update(); // بازگرداندن رنگ به حالت عادی هنگام خروج موس از محدوده قطعه
        QGraphicsItem::hoverLeaveEvent(event);
    }


    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override {

        Q_UNUSED(event);

        // کد دابل کلیک برای ویرایش خواص قطعه

        // ...

    }

};



// متدهای MainCanvas

MainCanvas::MainCanvas(QWidget* parent) : QGraphicsView(parent) {

    scene = new QGraphicsScene(this);

    setScene(scene);

    gridSize = 20;

    isPanning = false;

    leftDragging = false;

    currentZoom = 1.0;

    activeComponentType = "";

    activeWire = nullptr;

    hoveredPin = nullptr;

    floatingControlPanel = nullptr;

    lblSimStatus = nullptr;

    lblTimerDisplay = nullptr;


    undoStack = new QUndoStack(this);
    setRenderHint(QPainter::Antialiasing);

    setRenderHint(QPainter::SmoothPixmapTransform);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setMouseTracking(true);

    setBackgroundBrush(QColor::fromRgb(0x15, 0x15, 0x15));

    setCanvasSize("A4");



    setDragMode(QGraphicsView::RubberBandDrag);



    connect(scene, &QGraphicsScene::selectionChanged, this, [this]() {

        QList<QGraphicsItem*> selected = scene->selectedItems();

        if (!selected.isEmpty()) {

            QGraphicsComponentItem* compItem = dynamic_cast<QGraphicsComponentItem*>(selected.first());

            if (compItem) emit componentSelected(compItem->comp);

            else emit componentSelected(nullptr);

        } else {

            emit componentSelected(nullptr);

        }

    });



    isSimulating = false;

    simElapsedTenths = 0;



    simulationTimer = new QTimer(this);

    connect(simulationTimer, &QTimer::timeout, this, &MainCanvas::runSimulationStep);



    clockTimer = new QTimer(this);

    connect(clockTimer, &QTimer::timeout, this, &MainCanvas::updateClock);



    createFloatingControlPanel();
    updateZoomValue();

    current_circuit = 0;
    max_circuit = 0;
    saveToFile(QString("circuit_temp_%1.json").arg(current_circuit));

}



MainCanvas::~MainCanvas() {}



void MainCanvas::setCanvasSize(const QString& sizeStr) {

    if (sizeStr.contains("A3")) { scene->setSceneRect(0, 0, 1600, 1200); }

    else if (sizeStr.contains("Custom")) { scene->setSceneRect(0, 0, 1000, 1000); }

    else { scene->setSceneRect(0, 0, 1200, 900); }

}



QPointF MainCanvas::snapToGrid(const QPointF& pos) {

    qreal x = qRound(pos.x() / gridSize) * gridSize;

    qreal y = qRound(pos.y() / gridSize) * gridSize;

    return QPointF(x, y);

}



void MainCanvas::addComponent(const QString& type, const QPointF& pos) {

    QPointF snapped = snapToGrid(pos);

    static int counter = 1;

    QString id = type.left(3).toUpper() + QString::number(counter++);

    Component* comp = ComponentLibrary::getInstance().createComponent(type, id, snapped);

    if (comp) {

        QGraphicsComponentItem* item = new QGraphicsComponentItem(comp);

        scene->addItem(item);

        emit componentPlaced();
        saveStateForUndo();

    }

}



void MainCanvas::drawBackground(QPainter* painter, const QRectF& rect) {

    QGraphicsView::drawBackground(painter, rect);

    painter->save();

    int smallGrid = gridSize;

    int bigGrid = gridSize * 5;

    painter->setPen(QPen(QColor::fromRgb(0x30, 0x30, 0x30), 1));

    int left = int(rect.left()) - (int(rect.left()) % smallGrid);

    int top = int(rect.top()) - (int(rect.top()) % smallGrid);

    int right = int(rect.right());

    int bottom = int(rect.bottom());

    for (int x = left; x < right; x += smallGrid) {

        painter->drawLine(x, rect.top(), x, rect.bottom());

    }

    for (int y = top; y < bottom; y += smallGrid) {

        painter->drawLine(rect.left(), y, rect.right(), y);

    }

    painter->setPen(QPen(QColor::fromRgb(0x50, 0x50, 0x50), 1.5));

    int leftBig = int(rect.left()) - (int(rect.left()) % bigGrid);

    int topBig = int(rect.top()) - (int(rect.top()) % bigGrid);

    for (int x = leftBig; x < right; x += bigGrid) {

        painter->drawLine(x, rect.top(), x, rect.bottom());

    }

    for (int y = topBig; y < bottom; y += bigGrid) {

        painter->drawLine(rect.left(), y, rect.right(), y);

    }

    painter->setPen(QPen(QColor::fromRgb(0x00, 0xff, 0x88), 2));

    painter->drawLine(0, rect.top(), 0, rect.bottom());

    painter->drawLine(rect.left(), 0, rect.right(), 0);

    painter->restore();

}



void MainCanvas::wheelEvent(QWheelEvent* event) {

    double factor = 1.15;

    if (event->angleDelta().y() > 0) {

        if (currentZoom < 3) {

            scale(factor, factor);

            currentZoom *= factor;

        }

    } else {

        if (currentZoom > 0.3) {

            scale(1 / factor, 1 / factor);

            currentZoom /= factor;

        }

    }

    updateZoomValue();

}



Pin* MainCanvas::findPinAt(const QPointF& scenePos) {

    const qreal pinSensitivity = 10.0;

    for (QGraphicsItem* item : scene->items()) {

        QGraphicsComponentItem* compItem = dynamic_cast<QGraphicsComponentItem*>(item);

        if (compItem) {

            for (Pin* pin : compItem->comp->getPins()) {

                QPointF globalPos = pin->getGlobalPosition();

                qreal dist = QLineF(scenePos, globalPos).length();

                if (dist <= pinSensitivity) {

                    return pin;

                }

            }

        }

    }

    return nullptr;

}



void MainCanvas::mousePressEvent(QMouseEvent* event) {

    QPointF scenePos = mapToScene(event->pos());

    QPointF snappedPos = snapToGrid(scenePos);



    if (event->button() == Qt::RightButton && activeWire) {

        scene->removeItem(activeWire);

        delete activeWire;

        activeWire = nullptr;

        scene->update();

        event->accept();

        return;

    }



    if (event->button() == Qt::LeftButton) {

        Pin* clickedPin = findPinAt(scenePos);



        if (!activeWire) {

            if (clickedPin) {

                activeWire = new Wire(clickedPin);

                scene->addItem(activeWire);

                activeWire->setTempEndPoint(clickedPin->getGlobalPosition());

                event->accept();

                return;

            }

        }

        else {

            if (clickedPin) {

                if (clickedPin != activeWire->getStartPin() &&

                        clickedPin->getParentComponent() != activeWire->getStartPin()->getParentComponent()) {

                    activeWire->setEndPin(clickedPin);

                    wires.append(activeWire);

                    activeWire = nullptr;
                    saveStateForUndo();

                }

            }

            else {

                activeWire->addWayPoint(snappedPos);

            }

            scene->update();

            event->accept();

            return;

        }

    }



    if (event->button() == Qt::MiddleButton || (event->button() == Qt::RightButton && !activeWire)) {

        isPanning = true;

        lastMousePos = event->pos();

        setCursor(Qt::ClosedHandCursor);

        event->accept();

        return;

    }



    QGraphicsItem* item = scene->itemAt(scenePos, transform());

    if (event->button() == Qt::LeftButton && !activeComponentType.isEmpty() && !item && !activeWire) {

        addComponent(activeComponentType, scenePos);

        setCurrentSelectedType("");

        event->accept();

        return;

    }



    QGraphicsView::mousePressEvent(event);

}



void MainCanvas::mouseMoveEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());
    QPointF snappedPos = snapToGrid(scenePos); // اسنپ مختصات روی گرید

    emit mouseMoved(snappedPos); // ارسال مختصات اسنپ‌شده به MainWindow

    if (activeWire) {
        Pin* targetPin = findPinAt(scenePos);
        if (targetPin) {
            activeWire->setTempEndPoint(targetPin->getGlobalPosition());
        } else {
            activeWire->setTempEndPoint(scenePos);
        }
        event->accept();
        return;
    }

    if (isPanning) {
        QPoint delta = lastMousePos - event->pos();
        lastMousePos = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() + delta.y());
        event->accept();
        return;
    }

    Pin* pinUnderMouse = findPinAt(scenePos);
    if (pinUnderMouse != hoveredPin) {
        hoveredPin = pinUnderMouse;
        scene->update();
    }

    QGraphicsView::mouseMoveEvent(event);
}


void MainCanvas::mouseReleaseEvent(QMouseEvent* event) {

    if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {

        isPanning = false;

        setCursor(isWireMode ? Qt::CrossCursor : Qt::ArrowCursor);



        if (event->button() == Qt::RightButton && !activeWire && (event->pos() - lastMousePos).manhattanLength() < 5) {

            QMenu menu(this);

            ComponentLibrary& lib = ComponentLibrary::getInstance();

            for (const QString& cat : lib.getCategories()) {

                QMenu* subMenu = menu.addMenu(cat);

                for (const QString& compType : lib.getComponentsInCategory(cat)) {

                    QAction* act = subMenu->addAction(compType);

                    connect(act, &QAction::triggered, this, [this, compType, event]() {

                        QPointF scenePos = mapToScene(event->pos());

                        addComponent(compType, scenePos);

                    });

                }

            }

            menu.exec(event->globalPosition().toPoint());

        }

        event->accept();

        return;

    }



    QGraphicsView::mouseReleaseEvent(event);

}
void MainCanvas::keyPressEvent(QKeyEvent* event) {

    if (activeWire) {

        if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Escape) {

            if (!activeWire->removeLastWayPoint()) {

                scene->removeItem(activeWire);

                delete activeWire;

                activeWire = nullptr;
            }
            scene->update();
            event->accept();
            return;
        }
    }
    if (event->key() == Qt::Key_Delete) {

        QList<QGraphicsItem*> selected = scene->selectedItems();

        for (QGraphicsItem* item : selected) {

            Wire* wireItem = dynamic_cast<Wire*>(item);

            if (wireItem) {

                wires.removeAll(wireItem);

                scene->removeItem(wireItem);

                delete wireItem;

                continue;

            }



            QGraphicsComponentItem* compItem = dynamic_cast<QGraphicsComponentItem*>(item);

            if (compItem) {

                QVector<Wire*> wiresToRemove;

                for (Wire* wire : wires) {

                    if (wire->getStartPin()->getParentComponent() == compItem->comp ||

                        (wire->getEndPin() && wire->getEndPin()->getParentComponent() == compItem->comp)) {

                        wiresToRemove.append(wire);

                    }

                }

                for (Wire* w : wiresToRemove) {

                    wires.removeAll(w);

                    scene->removeItem(w);

                    delete w;

                }

                scene->removeItem(compItem);

                delete compItem->comp;

                delete compItem;

            }

        }
        saveStateForUndo();

        scene->update();

        event->accept();

        return;

    }
    if (event->key() == Qt::Key_R || ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_R)) {

        bool rotatedAny = false;

        for (QGraphicsItem* item : scene->selectedItems()) {

            QGraphicsComponentItem* compItem = dynamic_cast<QGraphicsComponentItem*>(item);
            if (compItem) {
                compItem->comp->rotateClockwise();
                compItem->update();
                for (Wire* wire : wires) {
                    if (wire->getStartPin()->getParentComponent() == compItem->comp ||

                        (wire->getEndPin() && wire->getEndPin()->getParentComponent() == compItem->comp)) {

                        wire->update();

                    }

                }
                rotatedAny = true;
                saveStateForUndo();
            }
        }
        if (!rotatedAny) {
            resetTransform();
            currentZoom = 1.0;
            updateZoomValue();
        }
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_F) {
        zoomToFit();
        event->accept();
        return;
    }
    QGraphicsView::keyPressEvent(event);

}
qreal MainCanvas::getZoomLevel() const { return currentZoom; }
void MainCanvas::updateZoomValue() { emit zoomChanged(int(currentZoom * 100)); }
void MainCanvas::setCurrentSelectedType(const QString& type) {

    activeComponentType = type;

    if (type.isEmpty()) {

        setDragMode(QGraphicsView::RubberBandDrag);

    } else {

        setDragMode(QGraphicsView::NoDrag);

    }

}
void MainCanvas::zoomToFit() {
    QRectF rect = scene->itemsBoundingRect();
    if (rect.isEmpty()) rect = scene->sceneRect();
    rect.adjust(-50, -50, 50, 50);
    fitInView(rect, Qt::KeepAspectRatio);
    currentZoom = transform().m11();
    updateZoomValue();
}
void MainCanvas::startSimulation() {
    isSimulating = true;
    if (!simulationTimer->isActive()) {
        simulationTimer->start(80);
    }

    if (!clockTimer->isActive()) {
        clockTimer->start(100);
    }
}
void MainCanvas::pauseSimulation() {
    isSimulating = false;
    simulationTimer->stop();
    clockTimer->stop();
}
void MainCanvas::stopSimulation() {
    pauseSimulation();
    simElapsedTenths = 0;
    updateTimerLabel();
}
void MainCanvas::runSimulationStep() {

    if (!isSimulating) return;
    for (Wire* wire : wires) {

        wire->propagateSignal();
    }
    for (QGraphicsItem* item : scene->items()) {

        QGraphicsComponentItem* compItem = dynamic_cast<QGraphicsComponentItem*>(item);

        if (compItem && compItem->comp) {

            compItem->comp->updateState();

            compItem->update();
        }
    }
    scene->update();
}
void MainCanvas::updateClock() {

    if (isSimulating) {

        simElapsedTenths++;

        updateTimerLabel();

    }

}
void MainCanvas::updateTimerLabel() {

    if (!lblTimerDisplay) return;
    int totalSeconds = simElapsedTenths / 10;
    int tenths = simElapsedTenths % 10;
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    QString timeText = QString("%1:%2:%3.%4")

                           .arg(hours, 2, 10, QChar('0'))
                           .arg(minutes, 2, 10, QChar('0'))
                           .arg(seconds, 2, 10, QChar('0'))
                           .arg(tenths);
    lblTimerDisplay->setText(timeText);
}
void MainCanvas::createFloatingControlPanel() {
    floatingControlPanel = new QWidget(this);
    floatingControlPanel->setStyleSheet(

        "QWidget {"

        "   background-color: rgba(18, 24, 38, 220);"

        "   border: 1px solid #00ff88;"

        "   border-radius: 8px;"

        "}"

        "QPushButton {"

        "   background-color: #1e293b;"

        "   color: #ffffff;"

        "   border: 1px solid #334155;"

        "   border-radius: 5px;"

        "   padding: 5px 10px;"
        "   font-weight: bold;"
        "   font-size: 11px;"

        "}"

        "QPushButton:hover {"

        "   background-color: #334155;"

        "}"
        );
    QHBoxLayout* layout = new QHBoxLayout(floatingControlPanel);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(8);
    lblSimStatus = new QLabel("STATUS: STOPPED", floatingControlPanel);
    lblSimStatus->setStyleSheet("color: #ff3366; font-weight: bold; border: none; font-size: 10px;");
    lblTimerDisplay = new QLabel("00:00:00.0", floatingControlPanel);
    lblTimerDisplay->setStyleSheet("color: #00d2ff; font-weight: bold; border: none; font-size: 11px; font-family: Consolas, monospace;");
    QPushButton* btnRun = new QPushButton("▶ RUN", floatingControlPanel);
    btnRun->setStyleSheet("QPushButton { color: #00ff88; } QPushButton:hover { background: #054422; }");
    QPushButton* btnPause = new QPushButton("⏸ PAUSE", floatingControlPanel);
    btnPause->setStyleSheet("QPushButton { color: #ffcc00; } QPushButton:hover { background: #443300; }");
    QPushButton* btnStop = new QPushButton("⏹ STOP", floatingControlPanel);

    btnStop->setStyleSheet("QPushButton { color: #ff3366; } QPushButton:hover { background: #440011; }");
    layout->addWidget(lblSimStatus);
    layout->addWidget(lblTimerDisplay);
    layout->addWidget(btnRun);
    layout->addWidget(btnPause);
    layout->addWidget(btnStop);
    connect(btnRun, &QPushButton::clicked, this, [this]() {
        startSimulation();
        lblSimStatus->setText("STATUS: RUNNING");
        lblSimStatus->setStyleSheet("color: #00ff88; font-weight: bold; border: none; font-size: 10px;");

    });
    connect(btnPause, &QPushButton::clicked, this, [this]() {
        pauseSimulation();
        lblSimStatus->setText("STATUS: PAUSED");
        lblSimStatus->setStyleSheet("color: #ffcc00; font-weight: bold; border: none; font-size: 10px;");

    });
    connect(btnStop, &QPushButton::clicked, this, [this]() {

        stopSimulation();

        lblSimStatus->setText("STATUS: STOPPED");

        lblSimStatus->setStyleSheet("color: #ff3366; font-weight: bold; border: none; font-size: 10px;");

    });
    floatingControlPanel->adjustSize();

}
void MainCanvas::resizeEvent(QResizeEvent* event) {

    QGraphicsView::resizeEvent(event);

    if (floatingControlPanel) {

        int x = width() - floatingControlPanel->width() - 20;

        int y = 20;

        floatingControlPanel->move(x, y);

    }

}
// ==========================================
// ۱. پاک‌سازی بوم قبل از لود پروژه جدید
// ==========================================
void MainCanvas::clearCanvas() {
    stopSimulation();

    // اگر سیمی در حال کشیده شدن بود آن را حذف کن
    if (activeWire) {
        scene->removeItem(activeWire);
        delete activeWire;
        activeWire = nullptr;
    }

    // پاک کردن تمام سیم‌ها
    for (Wire* w : wires) {
        scene->removeItem(w);
        delete w;
    }
    wires.clear();

    // پاک کردن تمام قطعات گرافیکی و اشیاء آن‌ها
    QList<QGraphicsItem*> allItems = scene->items();
    for (QGraphicsItem* item : allItems) {
        QGraphicsComponentItem* compItem = dynamic_cast<QGraphicsComponentItem*>(item);
        if (compItem) {
            scene->removeItem(compItem);
            delete compItem->comp; // حذف ساختار منطقی
            delete compItem;       // حذف المان گرافیکی
        }
    }

    scene->clear();
    scene->update();
}

// ==========================================
// ۲. ذخیره‌سازی پروژه در قالب فایل JSON
// ==========================================
bool MainCanvas::saveToFile(const QString &filePath) {
    QJsonObject rootObject;

    // ذخیره ابعاد بوم
    rootObject["canvasWidth"] = scene->sceneRect().width();
    rootObject["canvasHeight"] = scene->sceneRect().height();

    // ۱. ذخیره قطعات
    QJsonArray componentsArray;
    for (QGraphicsItem* item : scene->items()) {
        QGraphicsComponentItem* compItem = dynamic_cast<QGraphicsComponentItem*>(item);
        if (compItem && compItem->comp) {
            Component* comp = compItem->comp;
            QJsonObject compObj;
            compObj["id"] = comp->getId();
            compObj["type"] = comp->getName(); // یا نوع قطعه
            compObj["x"] = comp->getPosition().x();
            compObj["y"] = comp->getPosition().y();
            compObj["rotation"] = comp->getRotationAngle();

            componentsArray.append(compObj);
        }
    }
    rootObject["components"] = componentsArray;

    // ۲. ذخیره سیم‌ها و ارتباطات
    QJsonArray wiresArray;
    for (Wire* wire : wires) {
        if (!wire || !wire->getStartPin() || !wire->getEndPin()) continue;

        QJsonObject wireObj;
        // ذخیره پین ابتدا
        wireObj["startCompId"] = wire->getStartPin()->getParentComponent()->getId();
        wireObj["startPinId"] = wire->getStartPin()->getId();

        // ذخیره پین انتها
        wireObj["endCompId"] = wire->getEndPin()->getParentComponent()->getId();
        wireObj["endPinId"] = wire->getEndPin()->getId();

        wiresArray.append(wireObj);
    }
    rootObject["wires"] = wiresArray;

    // نوشتن روی فایل
    QJsonDocument doc(rootObject);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}
// ==========================================
// ۳. بازگردانی پروژه از روی فایل JSON
// ==========================================
bool MainCanvas::loadFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return false;

    // بوم فعلی را پاک می‌کنیم
    clearCanvas();

QJsonObject rootObject = doc.object();
    // ۱. بازسازی قطعات
    QJsonArray componentsArray = rootObject["components"].toArray();
    QMap<QString, Component*> createdComponents;

    for (const QJsonValue &val : componentsArray) {
        QJsonObject compObj = val.toObject();
        QString id = compObj["id"].toString();
        QString type = compObj["type"].toString();
        double x = compObj["x"].toDouble();
        double y = compObj["y"].toDouble();
        int rotation = compObj["rotation"].toInt();

        // ساخت قطعه از طریق ComponentLibrary
        Component* comp = ComponentLibrary::getInstance().createComponent(type, id, QPointF(x, y));
        if (comp) {
            // اعمال زاویه چرخش
            while (comp->getRotationAngle() != rotation) {
                comp->rotateClockwise();
            }

            QGraphicsComponentItem* item = new QGraphicsComponentItem(comp);
            scene->addItem(item);
            createdComponents[id] = comp;
        }
    }

    // تابع کمکی داخلی برای پیدا کردن پین با استفاده از ID قطعه و ID پین
    auto findPin = [&createdComponents](const QString &compId, const QString &pinId) -> Pin* {
        if (!createdComponents.contains(compId)) return nullptr;
        Component* comp = createdComponents[compId];
        for (Pin* p : comp->getPins()) {
            if (p->getId() == pinId) return p;
        }
        return nullptr;
    };

    // ۲. بازسازی سیم‌ها
    QJsonArray wiresArray = rootObject["wires"].toArray();
    for (const QJsonValue &val : wiresArray) {
        QJsonObject wireObj = val.toObject();
        QString startCompId = wireObj["startCompId"].toString();
        QString startPinId = wireObj["startPinId"].toString();
        QString endCompId = wireObj["endCompId"].toString();
        QString endPinId = wireObj["endPinId"].toString();

        Pin* startPin = findPin(startCompId, startPinId);
        Pin* endPin = findPin(endCompId, endPinId);

        if (startPin && endPin) {
            Wire* newWire = new Wire(startPin);
            newWire->setEndPin(endPin);
            scene->addItem(newWire);
            wires.append(newWire);
        }
    }

    scene->update();
    return true;
}
void MainCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    // ۱. پیدا کردن قطعه کلیک‌شده روی بوم
    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem* clickedItem = scene->itemAt(scenePos, QTransform());

    QGraphicsComponentItem* compItem = nullptr;
    while (clickedItem) {
        compItem = dynamic_cast<QGraphicsComponentItem*>(clickedItem);
        if (compItem) break;
        clickedItem = clickedItem->parentItem();
    }

    if (!compItem || !compItem->comp) {
        QGraphicsView::mouseDoubleClickEvent(event);
        return;
    }

    Component* selectedComp = compItem->comp;

    // ۲. تشخیص خودکار و هوشمند یکا (SI Unit) بر اساس نوع قطعه
    QString currentUnit = selectedComp->getUnit();
    QString compName = selectedComp->getName().toLower();

    if (currentUnit.isEmpty()) {
        if (compName.contains("resistor") || compName.contains("مقاومت")) {
            currentUnit = "Ω (اهم)";
        } else if (compName.contains("capacitor") || compName.contains("خازن")) {
            currentUnit = "F (فاراد)";
        } else if (compName.contains("inductor") || compName.contains("سلف")) {
            currentUnit = "H (هانری)";
        } else if (compName.contains("voltage") || compName.contains("dc") || compName.contains("battery") || compName.contains("ولتاژ")) {
            currentUnit = "V (ولت)";
        } else if (compName.contains("current") || compName.contains("جریان")) {
            currentUnit = "A (آمپر)";
        } else if (compName.contains("clock") || compName.contains("pulse") || compName.contains("فرکانس")) {
            currentUnit = "Hz (هرتز)";
        } else if (compName.contains("and") || compName.contains("or") || compName.contains("not") || compName.contains("nand") || compName.contains("xor")) {
            currentUnit = "ns (تأخیر)";
        } else {
            currentUnit = "—";
        }
        selectedComp->setUnit(currentUnit); // ثبت یکای جدید در شیء قطعه
    }

    // ۳. ساخت پنجره تنظیمات
    QDialog dialog(this);
    dialog.setWindowTitle("تنظیمات: " + selectedComp->getName());
    dialog.setFixedWidth(390);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // ۴. استایل‌دهی مدرن با فلش‌های کاملاً واضح برای SpinBox
    dialog.setStyleSheet(
        "QDialog {"
        "   background-color: #080e1c;"
        "   color: #e2e8f0;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        "QLabel {"
        "   color: #94a3b8;"
        "   font-size: 13px;"
        "   font-weight: 500;"
        "}"
        "QLineEdit, QDoubleSpinBox {"
        "   background-color: #0f172a;"
        "   border: 1px solid rgba(0, 243, 255, 0.4);"
        "   border-radius: 6px;"
        "   color: #00f3ff;"
        "   padding: 6px 8px;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "QLineEdit:focus, QDoubleSpinBox:focus {"
        "   border: 1px solid #00f3ff;"
        "   background-color: #1e293b;"
        "}"
        /* --- اصلاح و نمایش واضح دکمه‌های بالا/پایین QDoubleSpinBox --- */
        "QDoubleSpinBox::up-button {"
        "   subcontrol-origin: border;"
        "   subcontrol-position: top right;"
        "   width: 22px;"
        "   background-color: rgba(0, 243, 255, 0.15);"
        "   border-top-right-radius: 5px;"
        "   border-bottom: 1px solid #080e1c;"
        "}"
        "QDoubleSpinBox::down-button {"
        "   subcontrol-origin: border;"
        "   subcontrol-position: bottom right;"
        "   width: 22px;"
        "   background-color: rgba(0, 243, 255, 0.15);"
        "   border-bottom-right-radius: 5px;"
        "}"
        "QDoubleSpinBox::up-button:hover, QDoubleSpinBox::down-button:hover {"
        "   background-color: #00f3ff;"
        "}"
        /* ساخت فلش‌های مثلثی با CSS جهت دیده‌شدن عالی */
        "QDoubleSpinBox::up-arrow {"
        "   width: 0; height: 0;"
        "   border-left: 4px solid transparent;"
        "   border-right: 4px solid transparent;"
        "   border-bottom: 6px solid #00f3ff;"
        "}"
        "QDoubleSpinBox::down-arrow {"
        "   width: 0; height: 0;"
        "   border-left: 4px solid transparent;"
        "   border-right: 4px solid transparent;"
        "   border-top: 6px solid #00f3ff;"
        "}"
        "QDoubleSpinBox::up-arrow:hover {"
        "   border-bottom-color: #080e1c;"
        "}"
        "QDoubleSpinBox::down-arrow:hover {"
        "   border-top-color: #080e1c;"
        "}"
        /* ------------------------------------------------------------- */
        "QPushButton {"
        "   background-color: rgba(0, 243, 255, 0.1);"
        "   color: #00f3ff;"
        "   border: 1px solid rgba(0, 243, 255, 0.5);"
        "   border-radius: 6px;"
        "   padding: 7px 18px;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #00f3ff;"
        "   color: #050b14;"
        "}"
        );

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // ۵. تیتر پنجره
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("⚙️  تنظیمات " + selectedComp->getName(), &dialog);
    titleLabel->setStyleSheet("color: #00f3ff; font-size: 15px; font-weight: bold;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    QFrame* line = new QFrame(&dialog);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: rgba(0, 243, 255, 0.2);");
    mainLayout->addWidget(line);

    // ۶. فرم مشخصات
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(12);

    // کادر ID
    QLineEdit* editId = new QLineEdit(selectedComp->getId(), &dialog);
    formLayout->addRow("شناسه / لیبل (ID):", editId);

    // کادر Value و نمایش یکا
    QHBoxLayout* valLayout = new QHBoxLayout();
    QDoubleSpinBox* spinValue = new QDoubleSpinBox(&dialog);
    spinValue->setRange(-1e9, 1e9);
    spinValue->setDecimals(2);
    spinValue->setValue(selectedComp->getValue());

    QLabel* unitLabel = new QLabel(currentUnit, &dialog);
    unitLabel->setStyleSheet("color: #39FF14; font-weight: bold; font-size: 13px; padding-left: 5px;");

    valLayout->addWidget(spinValue, 1);
    valLayout->addWidget(unitLabel);
    formLayout->addRow("مقدار اصلی (Value):", valLayout);

    mainLayout->addLayout(formLayout);

    // ۷. کارت مشخصات فنی سیستم
    QFrame* infoCard = new QFrame(&dialog);
    infoCard->setStyleSheet(
        "QFrame {"
        "   background-color: rgba(15, 23, 42, 0.7);"
        "   border: 1px solid rgba(255, 255, 255, 0.08);"
        "   border-radius: 8px;"
        "}"
        );
    QVBoxLayout* cardLayout = new QVBoxLayout(infoCard);
    cardLayout->setContentsMargins(12, 10, 12, 10);

    QLabel* cardTitle = new QLabel("ℹ️ اطلاعات قطعه", infoCard);
    cardTitle->setStyleSheet("color: #38bdf8; font-weight: bold; font-size: 11px;");
    cardLayout->addWidget(cardTitle);

    QLabel* cardText = new QLabel(infoCard);
    cardText->setWordWrap(true);
    cardText->setStyleSheet("color: #94a3b8; font-size: 11px; background: transparent;");
    cardText->setText(QString("• دسته: %1\n• موقعیت روی بوم: X=%2 , Y=%3")
                          .arg(selectedComp->getName())
                          .arg((int)selectedComp->getPosition().x())
                          .arg((int)selectedComp->getPosition().y()));
    cardLayout->addWidget(cardText);

    mainLayout->addWidget(infoCard);

    // ۸. دکمه‌های تایید و انصراف
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttonBox->button(QDialogButtonBox::Ok)->setText("ثبت تغییرات");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("انصراف");

    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // ۹. اعمال تغییرات
    if (dialog.exec() == QDialog::Accepted) {
        selectedComp->setId(editId->text().trimmed());
        selectedComp->setValue(spinValue->value());
        scene->update(); // بروزرسانی بوم
        saveStateForUndo();
    }
}
void MainCanvas::exportToImage() {
    // ۱. دریافت مسیر و فرمت ذخیره‌سازی از کاربر
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "ذخیره تصویر مدار",
        "",
        "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg)"
        );

    if (filePath.isEmpty()) return;

    // ۲. محاسبه محدوده کل آیتم‌های موجود روی بوم (با حاشیه مناسب)
    QRectF bounds = scene->itemsBoundingRect();
    if (bounds.isEmpty()) {
        bounds = scene->sceneRect();
    }
    bounds.adjust(-20, -20, 20, 20); // ۲ اوت‌پت حاشیه برای زیبایی تصویر

    // ۳. ساخت تصویر خروجی و رسم محتوای scene روی آن
    QImage image(bounds.size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::white); // پس‌زمینه سفید

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter, QRectF(), bounds);
    painter.end();

    // ۴. ذخیره فایل روی سیستم
    if (image.save(filePath)) {
        QMessageBox::information(this, "موفقیت", "تصویر مدار با موفقیت ذخیره شد.");
    } else {
        QMessageBox::critical(this, "خطا", "خطا در ذخیره‌سازی فایل تصویر!");
    }
}

void MainCanvas::zoomIn() {
    double factor = 1.15;
    if (currentZoom < 3) {
        scale(factor, factor);
        currentZoom *= factor;
        updateZoomValue();
    }
}

void MainCanvas::zoomOut() {
    double factor = 1.15;
    if (currentZoom > 0.3) {
        scale(1 / factor, 1 / factor);
        currentZoom /= factor;
        updateZoomValue();
    }
}

void MainCanvas::toggleWireMode() {
    isWireMode = !isWireMode;
    if (isWireMode) {
        setCursor(Qt::CrossCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

void MainCanvas::saveStateForUndo() {
    current_circuit += 1;
    max_circuit = current_circuit;
    QString filename = QString("circuit_temp_%1.json").arg(current_circuit);
    saveToFile(filename);
}

void MainCanvas::undo() {
    if (current_circuit > 0) {
        current_circuit -= 1;
        QString filename = QString("circuit_temp_%1.json").arg(current_circuit);
        loadFromFile(filename);
    }
}

void MainCanvas::redo() {
    if (current_circuit < max_circuit) {
        current_circuit += 1;
        QString filename = QString("circuit_temp_%1.json").arg(current_circuit);
        loadFromFile(filename);
    }
}
