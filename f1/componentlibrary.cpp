#include "componentlibrary.h"
#include "basecomponents.h"

ComponentLibrary& ComponentLibrary::getInstance() {
    static ComponentLibrary instance;
    return instance;
}

ComponentLibrary::ComponentLibrary() {
    // اصلاح رشته‌ها برای انطباق دقیق با سیستم پیش‌نمایش و بوم
    libraryData["Analog"] = QStringList() << "Resistor" << "Capacitor" << "Inductor" << "Switch" << "Button" << "LED" << "7-Segment";
    libraryData["Digital"] = QStringList() << "AND" << "OR" << "NOT" << "NAND" << "XOR" << "D Flip-Flop";
    libraryData["Sources"] = QStringList() << "GND" << "VoltageSource";
}

ComponentLibrary::~ComponentLibrary() {}

QStringList ComponentLibrary::getCategories() const {
    return libraryData.keys();
}

QStringList ComponentLibrary::getComponentsInCategory(const QString& category) const {
    return libraryData.value(category, QStringList());
}

Component* ComponentLibrary::createComponent(const QString& type, const QString& id, const QPointF& pos) {
    if (type == "GND") return new GndComponent(id, pos);
    if (type == "VoltageSource") return new VoltageSourceComponent(id, pos, 5.0);
    if (type == "Resistor") return new ResistorComponent(id, pos, 1000.0);
    if (type == "Capacitor") return new CapacitorComponent(id, pos, 1e-6);
    if (type == "Inductor") return new InductorComponent(id, pos, 1e-3);
    if (type == "Switch") return new SwitchComponent(id, pos);
    if (type == "Button") return new ButtonComponent(id, pos);
    if (type == "LED") return new LedComponent(id, pos);
    if (type == "7-Segment") return new SevenSegmentComponent(id, pos);
    if (type == "AND") return new AndGateComponent(id, pos);
    if (type == "OR") return new OrGateComponent(id, pos);
    if (type == "NOT") return new NotGateComponent(id, pos);
    if (type == "NAND") return new NandGateComponent(id, pos);
    if (type == "XOR") return new XorGateComponent(id, pos);
    if (type == "D Flip-Flop") return new DFlipFlopComponent(id, pos);
    return nullptr;
}