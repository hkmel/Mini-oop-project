#include "maincanvas.h"
#include "componentlibrary.h"
#include "basecomponents.h"
#include "pin.h"
#include "wire.h"
#include <QGraphicsItem>
#include <QPainter>
#include <QScrollBar>
#include <QtMath>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QMessageBox>

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
            painter->drawLine(-30, 0, -15, 0);
            painter->drawRect(-15, -7, 30, 14);
            painter->drawLine(15, 0, 30, 0);
        } else if (name == "Capacitor") {
            painter->drawLine(-30, 0, -6, 0);
            painter->drawLine(-6, -15, -6, 15);
            painter->drawLine(6, -15, 6, 15);
            painter->drawLine(6, 0, 30, 0);
        } else if (name == "Inductor") {
            painter->drawLine(-30, 0, -20, 0);
            painter->drawArc(-20, -8, 10, 16, 0, 180 * 16);
            painter->drawArc(-10, -8, 10, 16, 0, 180 * 16);
            painter->drawArc(0, -8, 10, 16, 0, 180 * 16);
            painter->drawArc(10, -8, 10, 16, 0, 180 * 16);
            painter->drawLine(20, 0, 30, 0);
        } else if (name == "GND") {
            painter->drawLine(0, -10, 0, 10);
            painter->drawLine(-15, 10, 15, 10);
            painter->drawLine(-10, 15, 10, 15);
            painter->drawLine(-5, 20, 5, 20);
        } else if (name == "VoltageSource") {
            painter->drawEllipse(-15, -15, 30, 30);
            painter->drawLine(0, -20, 0, -15);
            painter->drawLine(0, 15, 0, 20);
            painter->drawText(QRectF(-15, -12, 30, 12), Qt::AlignCenter, "+");
            painter->drawText(QRectF(-15, 0, 30, 12), Qt::AlignCenter, "-");
        } else if (name == "Switch") {
            painter->drawLine(-30, 0, -15, 0);
            painter->drawEllipse(-15, -2, 4, 4);
            painter->drawEllipse(11, -2, 4, 4);
            SwitchComponent* sw = dynamic_cast<SwitchComponent*>(comp);
            if (sw && sw->isOpen()) painter->drawLine(-13, 0, 10, -12);
            else painter->drawLine(-13, 0, 11, 0);
            painter->drawLine(15, 0, 30, 0);
        } else if (name == "Button") {
            painter->drawLine(-30, 0, -15, 0);
            painter->drawEllipse(-15, -2, 4, 4);
            painter->drawEllipse(11, -2, 4, 4);
            ButtonComponent* btn = dynamic_cast<ButtonComponent*>(comp);
            if (btn && btn->isPressed()) painter->drawLine(-13, 0, 11, 0);
            else painter->drawLine(-13, -6, 11, -6);
            painter->drawLine(-1, -6, -1, -12);
            painter->drawLine(-8, -12, 6, -12);
            painter->drawLine(15, 0, 30, 0);
        } else if (name == "AND") {
            painter->drawLine(-30, -10, -15, -10);
            painter->drawLine(-30, 10, -15, 10);
            QPainterPath path;
            path.moveTo(-15, -20);
            path.lineTo(0, -20);
            path.arcTo(-15, -20, 40, 40, 90, -180);
            path.lineTo(-15, 20);
            path.closeSubpath();
            painter->drawPath(path);
            painter->drawLine(25, 0, 30, 0);
        } else if (name == "OR") {
            painter->drawLine(-30, -10, -11, -10);
            painter->drawLine(-30, 10, -11, 10);
            QPainterPath path;
            path.moveTo(-15, -20);
            path.arcTo(-25, -20, 20, 40, 90, -180);
            path.arcTo(-45, -20, 70, 40, 90, -90);
            path.arcTo(-45, -20, 70, 40, 0, -90);
            path.closeSubpath();
            painter->drawPath(path);
            painter->drawLine(25, 0, 30, 0);
        } else if (name == "NOT") {
            painter->drawLine(-30, 0, -15, 0);
            QPainterPath path;
            path.moveTo(-15, -15);
            path.lineTo(10, 0);
            path.lineTo(-15, 15);
            path.closeSubpath();
            painter->drawPath(path);
            painter->drawEllipse(10, -3, 6, 6);
            painter->drawLine(16, 0, 30, 0);
        } else if (name == "NAND") {
            painter->drawLine(-30, -10, -15, -10);
            painter->drawLine(-30, 10, -15, 10);
            QPainterPath path;
            path.moveTo(-15, -20);
            path.lineTo(0, -20);
            path.arcTo(-15, -20, 40, 40, 90, -180);
            path.lineTo(-15, 20);
            path.closeSubpath();
            painter->drawPath(path);
            painter->drawEllipse(25, -3, 6, 6);
            painter->drawLine(31, 0, 35, 0);
        } else if (name == "XOR") {
            painter->drawLine(-30, -10, -13, -10);
            painter->drawLine(-30, 10, -13, 10);
            QPainterPath path;
            path.moveTo(-15, -20);
            path.arcTo(-25, -20, 20, 40, 90, -180);
            path.arcTo(-45, -20, 70, 40, 90, -90);
            path.arcTo(-45, -20, 70, 40, 0, -90);
            path.closeSubpath();
            painter->drawPath(path);
            painter->drawArc(-29, -20, 20, 40, 90, -180);
            painter->drawLine(25, 0, 30, 0);
        } else if (name == "D Flip-Flop") {
            painter->drawRect(-20, -25, 40, 50);
            painter->drawLine(-20, -10, -30, -10);
            painter->drawLine(-20, 10, -30, 10);
            painter->drawLine(20, -10, 30, -10);
            painter->drawLine(20, 10, 30, 10);
            painter->drawText(QRectF(-18, -18, 15, 15), Qt::AlignCenter, "D");
            painter->drawText(QRectF(-18, 2, 20, 15), Qt::AlignCenter, "C");
            painter->drawText(QRectF(2, -18, 15, 15), Qt::AlignCenter, "Q");
            painter->drawText(QRectF(2, 2, 18, 15), Qt::AlignCenter, "QB");
        } else if (name == "LED") {
            painter->drawLine(-30, 0, -10, 0);
            QPainterPath path;
            path.moveTo(-10, -12);
            path.lineTo(10, 0);
            path.lineTo(-10, 12);
            path.closeSubpath();
            painter->drawPath(path);
            painter->drawLine(10, -12, 10, 12);
            painter->drawLine(10, 0, 30, 0);
            painter->drawLine(-3, -12, 5, -20);
            painter->drawLine(5, -20, 2, -19);
            painter->drawLine(5, -20, 4, -17);
        } else if (name == "7-Segment") {
            painter->drawRect(-25, -35, 50, 70);
        } else {
            painter->drawRect(-30, -30, 60, 60);
            painter->drawText(QRectF(-30, -25, 60, 20), Qt::AlignCenter, comp->getName());
        }
        painter->restore();
        painter->setPen(Qt::yellow);
        painter->setFont(QFont("Arial", 8));
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

            // فراخوانی متد کمکی بوم برای آپدیت اتصالات کل قطعات انتخاب شده
            MainCanvas* canvas = dynamic_cast<MainCanvas*>(scene()->views().first());
            if (canvas) {
                // ریپید زنده بر اساس متد بهینه‌سازی شده پایین
                QMetaObject::invokeMethod(canvas, "updateConnectedWiresForSelected", Qt::QueuedConnection);
            }
            return QPointF(x, y);
        }
        return QGraphicsItem::itemChange(change, value);
    }
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override {
        Q_UNUSED(event);
        QString name = comp->getName();
        if (name == "Resistor") {
            ResistorComponent* r = dynamic_cast<ResistorComponent*>(comp);
            if (r) {
                bool ok;
                double val = QInputDialog::getDouble(nullptr, "Edit Value", "Resistance (Ohm):", r->getResistance(), 0, 1e9, 2, &ok);
                if (ok) r->setResistance(val);
            }
        } else if (name == "Capacitor") {
            CapacitorComponent* c = dynamic_cast<CapacitorComponent*>(comp);
            if (c) {
                bool ok;
                double val = QInputDialog::getDouble(nullptr, "Edit Value", "Capacitance (F):", c->getCapacitance(), 0, 1e9, 9, &ok);
                if (ok) c->setCapacitance(val);
            }
        } else if (name == "Inductor") {
            InductorComponent* l = dynamic_cast<InductorComponent*>(comp);
            if (l) {
                bool ok;
                double val = QInputDialog::getDouble(nullptr, "Edit Value", "Inductance (H):", l->getInductance(), 0, 1e9, 9, &ok);
                if (ok) l->setInductance(val);
            }
        } else if (name == "VoltageSource") {
            VoltageSourceComponent* v = dynamic_cast<VoltageSourceComponent*>(comp);
            if (v) {
                bool ok;
                double val = QInputDialog::getDouble(nullptr, "Edit Value", "Voltage (V):", v->getVoltage(), -1e9, 1e9, 2, &ok);
                if (ok) v->setVoltage(val);
            }
        } else if (name == "Switch") {
            SwitchComponent* sw = dynamic_cast<SwitchComponent*>(comp);
            if (sw) { sw->toggle(); update(); }
        } else if (name == "Button") {
            ButtonComponent* btn = dynamic_cast<ButtonComponent*>(comp);
            if (btn) {
                if (btn->isPressed()) btn->release();
                else btn->press();
                update();
            }
        }
    }
};

MainCanvas::MainCanvas(QWidget* parent) : QGraphicsView(parent) {
    scene = new QGraphicsScene(this);
    setScene(scene);
    gridSize = 20;
    isPanning = false;
    currentZoom = 1.0;
    activeComponentType = "";
    activeWire = nullptr;
    hoveredPin = nullptr;
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);
    setBackgroundBrush(QColor::fromRgb(0x15, 0x15, 0x15));
    setCanvasSize("A4");

    // حالت درگ پیش‌فرض برای فعال‌سازی RubberBand (انتخاب مستطیلی چندتایی)
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

// متد کلیدی برای بروزرسانی سیم‌های متصل به گروه قطعات در حال جابه‌جایی
void MainCanvas::updateConnectedWiresForSelected() {
    for (Wire* wire : wires) {
        bool startConnected = false;
        bool endConnected = false;

        for (QGraphicsItem* item : scene->selectedItems()) {
            QGraphicsComponentItem* compItem = dynamic_cast<QGraphicsComponentItem*>(item);
            if (compItem) {
                if (wire->getStartPin()->getParentComponent() == compItem->comp) startConnected = true;
                if (wire->getEndPin() && wire->getEndPin()->getParentComponent() == compItem->comp) endConnected = true;
            }
        }
        if (startConnected || endConnected) {
            wire->update();
        }
    }
}

void MainCanvas::mousePressEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());

    if (event->button() == Qt::RightButton && activeWire) {
        scene->removeItem(activeWire);
        delete activeWire;
        activeWire = nullptr;
        scene->update();
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        // الف) قرار دادن قطعه جدید از لیست فعال روی فضای خالی بوم
        // الف) قرار دادن قطعه جدید از لیست فعال روی فضای خالی بوم
        // الف) قرار دادن قطعه جدید از لیست فعال روی فضای خالی بوم
        QGraphicsItem* item = scene->itemAt(scenePos, transform());
        if (!activeComponentType.isEmpty() && !item && !activeWire) {
            addComponent(activeComponentType, scenePos);

            activeComponentType = "";
            emit componentPlaced(); // 🌟 خط جدید: ارسال سیگنال به MainWindow

            event->accept();
            return;
        }
        // ب) مدیریت سیم‌کشی اتصالات
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
                }
            }
            else {
                QPointF snappedPos = snapToGrid(scenePos);
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

    // تغییر مود درگ به حالت RubberBand فقط اگر روی آیتمی کلیک نشده باشد
    QGraphicsItem* clickedItem = scene->itemAt(scenePos, transform());
    if (event->button() == Qt::LeftButton && !clickedItem && activeComponentType.isEmpty() && !activeWire) {
        setDragMode(QGraphicsView::RubberBandDrag);
    } else if (event->button() == Qt::LeftButton && clickedItem) {
        setDragMode(QGraphicsView::NoDrag); // اجازه بده فریم‌ورک جابه‌جایی المان‌ها رو انجام بده
    }

    QGraphicsView::mousePressEvent(event);
}

void MainCanvas::mouseMoveEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());
    emit mouseMoved(scenePos);

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
        setCursor(Qt::ArrowCursor);

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

    // بعد از اتمام درگ مستطیلی، مود درگ را ریست کن تا المان‌ها مستقیماً قابل کلیک و تکان دادن باشند
    setDragMode(QGraphicsView::RubberBandDrag);
    QGraphicsView::mouseReleaseEvent(event);
}

void MainCanvas::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_R) {
        bool rotatedAny = false;
        for (QGraphicsItem* item : scene->selectedItems()) {
            QGraphicsComponentItem* compItem = dynamic_cast<QGraphicsComponentItem*>(item);
            if (compItem) {
                compItem->comp->rotateClockwise();
                compItem->update();
                rotatedAny = true;
            }
        }
        if (rotatedAny) {
            updateConnectedWiresForSelected();
        } else {
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

    // 🌟 قابلیت حذف چندتایی و پاک کردن سیم‌های متصل به سبک پروتئوس 🌟
    if (event->key() == Qt::Key_Delete) {
        QList<QGraphicsItem*> itemsToRemove = scene->selectedItems();
        QVector<Wire*> wiresToRemove;

        // مرحله اول: پیدا کردن تمام سیم‌هایی که به قطعات در حال حذف متصل هستند
        for (QGraphicsItem* item : itemsToRemove) {
            QGraphicsComponentItem* compItem = dynamic_cast<QGraphicsComponentItem*>(item);
            if (compItem) {
                for (Wire* wire : wires) {
                    if (wire->getStartPin()->getParentComponent() == compItem->comp ||
                        (wire->getEndPin() && wire->getEndPin()->getParentComponent() == compItem->comp)) {
                        if (!wiresToRemove.contains(wire)) {
                            wiresToRemove.append(wire);
                        }
                    }
                }
            }
        }

        // مرحله دوم: حذف فیزیکی و امحای سیم‌های پیدا شده از حافظه و صحنه
        for (Wire* w : wiresToRemove) {
            wires.removeAll(w);
            scene->removeItem(w);
            delete w;
        }

        // مرحله سوم: حذف و امحای قطعات انتخاب شده
        for (QGraphicsItem* item : itemsToRemove) {
            QGraphicsComponentItem* compItem = dynamic_cast<QGraphicsComponentItem*>(item);
            if (compItem) {
                scene->removeItem(compItem);
                delete compItem->comp;
                delete compItem;
            }
        }

        scene->clearSelection();
        scene->update();
        event->accept();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}

qreal MainCanvas::getZoomLevel() const { return currentZoom; }
void MainCanvas::updateZoomValue() { emit zoomChanged(int(currentZoom * 100)); }

void MainCanvas::setCurrentSelectedType(const QString& type) {
    activeComponentType = type;
}

void MainCanvas::setActiveComponentType(const QString& type) { activeComponentType = type; }

void MainCanvas::zoomToFit() {
    QRectF rect = scene->itemsBoundingRect();
    if (rect.isEmpty()) rect = scene->sceneRect();
    rect.adjust(-50, -50, 50, 50);
    fitInView(rect, Qt::KeepAspectRatio);
    currentZoom = transform().m11();
    updateZoomValue();
}