#ifndef COMPONENTPROPERTIESDIALOG_H
#define COMPONENTPROPERTIESDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "component.h"

class ComponentPropertiesDialog : public QDialog {
    Q_OBJECT

public:
    explicit ComponentPropertiesDialog(Component* comp, QWidget *parent = nullptr)
        : QDialog(parent), m_comp(comp)
    {
        setWindowTitle(tr("ویرایش ویژگی‌های قطعه - %1").arg(comp->getName()));
        setFixedWidth(400);
        setupUI();
        applyStyle();
    }

private:
    Component* m_comp;
    QLineEdit* editId;
    QDoubleSpinBox* spinValue;
    QLabel* labelUnit;
    QTextEdit* textDescription;

    void setupUI() {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(12);

        // فرم مشخصات قابل ویرایش
        QFormLayout* formLayout = new QFormLayout();
        formLayout->setSpacing(10);

        // ۱. ویرایش نام / لیبل (ID)
        editId = new QLineEdit(m_comp->getId(), this);
        formLayout->addRow(new QLabel(tr("شناسه / لیبل (ID):"), this), editId);

        // ۲. ویرایش مقدار عددی (Value) و نمایش یکا (Unit)
        QHBoxLayout* valLayout = new QHBoxLayout();
        spinValue = new QDoubleSpinBox(this);
        spinValue->setRange(-1e9, 1e9);
        spinValue->setDecimals(3);
        spinValue->setValue(m_comp->getValue());

        labelUnit = new QLabel(m_comp->getUnit().isEmpty() ? "—" : m_comp->getUnit(), this);
        labelUnit->setStyleSheet("font-weight: bold; color: #00f3ff;");

        valLayout->addWidget(spinValue, 1);
        valLayout->addWidget(labelUnit);

        formLayout->addRow(new QLabel(tr("مقدار اصلی (Value):"), this), valLayout);
        mainLayout->addLayout(formLayout);

        // ۳. توضیحات مختصر درباره قطعه
        QGroupBox* descBox = new QGroupBox(tr("توضیحات قطعه"), this);
        QVBoxLayout* descLayout = new QVBoxLayout(descBox);

        textDescription = new QTextEdit(this);
        textDescription->setReadOnly(true);
        textDescription->setText(getDescriptionForComponent(m_comp->getName()));
        textDescription->setMaximumHeight(90);

        descLayout->addWidget(textDescription);
        descBox->setLayout(descLayout);
        mainLayout->addWidget(descBox);

        // ۴. دکمه‌های تایید و انصراف
        QHBoxLayout* btnLayout = new QHBoxLayout();
        QPushButton* btnOk = new QPushButton(tr("ثبت تغییرات"), this);
        QPushButton* btnCancel = new QPushButton(tr("انصراف"), this);

        btnOk->setCursor(Qt::PointingHandCursor);
        btnCancel->setCursor(Qt::PointingHandCursor);

        btnLayout->addStretch();
        btnLayout->addWidget(btnCancel);
        btnLayout->addWidget(btnOk);
        mainLayout->addLayout(btnLayout);

        // اتصالات
        connect(btnOk, &QPushButton::clicked, this, &ComponentPropertiesDialog::onAccept);
        connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    }

    // متد کمکی برای تولید توضیحات بر اساس نوع قطعه
    QString getDescriptionForComponent(const QString& name) {
        if (name.contains("Resistor", Qt::CaseInsensitive) || name.contains("مقاومت"))
            return "مقاومت (Resistor): قطعه‌ای پسیو که در برابر عبور جریان الکتریکی مقاومت کرده و طبق قانون اهم (V = I × R) باعث افت ولتاژ می‌شود.";
        if (name.contains("Capacitor", Qt::CaseInsensitive) || name.contains("خازن"))
            return "خازن (Capacitor): قطعه‌ای برای ذخیره‌سازی انرژی الکتریکی در میدان الکترواستاتیکی بین صفحات رسانا (I = C × dv/dt).";
        if (name.contains("Inductor", Qt::CaseInsensitive) || name.contains("سلف"))
            return "سلف / القاگر (Inductor): قطعه‌ای پسیو که انرژی را در میدان مغناطیسی ذخیره کرده و در برابر تغییرات جریان مقاومت نشان می‌دهد.";
        if (name.contains("DC Source", Qt::CaseInsensitive) || name.contains("Battery"))
            return "منبع ولتاژ مستقیم (DC Source): تامین‌کننده اختلاف پتانسیل ثات الکتریکی در مدار.";
        if (name.contains("AND", Qt::CaseInsensitive))
            return "گیت منطقی AND: خروجی این قطعه تنها زمانی ۱ (High) می‌شود که تمامی ورودی‌های آن در سطح ۱ باشند.";
        if (name.contains("OR", Qt::CaseInsensitive))
            return "گیت منطقی OR: خروجی این قطعه زمانی ۱ (High) می‌شود که حداقل یکی از ورودی‌های آن در سطح ۱ باشد.";
        if (name.contains("NOT", Qt::CaseInsensitive) || name.contains("Inverter"))
            return "گیت منطقی NOT: سطح منطقی سیگنال ورودی را معکوس (Invert) می‌کند.";

        return QString("قطعه الکترونیکی %1 جهت استفاده در شبیه‌ساز مدار.").arg(name);
    }

    void applyStyle() {
        setStyleSheet(
            "QDialog {"
            "   background-color: #0b132b;"
            "   color: #e2e8f0;"
            "   font-family: 'Segoe UI', sans-serif;"
            "}"
            "QLabel {"
            "   color: #cbd5e1;"
            "   font-size: 13px;"
            "}"
            "QLineEdit, QDoubleSpinBox, QTextEdit {"
            "   background-color: #1c2541;"
            "   border: 1px solid #3a506b;"
            "   border-radius: 5px;"
            "   color: #00f3ff;"
            "   padding: 5px;"
            "   font-size: 13px;"
            "}"
            "QLineEdit:focus, QDoubleSpinBox:focus {"
            "   border: 1px solid #00f3ff;"
            "}"
            "QGroupBox {"
            "   color: #00f3ff;"
            "   border: 1px solid #3a506b;"
            "   border-radius: 6px;"
            "   margin-top: 10px;"
            "   font-weight: bold;"
            "}"
            "QGroupBox::title {"
            "   subcontrol-origin: margin;"
            "   left: 10px;"
            "   padding: 0 5px;"
            "}"
            "QPushButton {"
            "   background-color: #1c2541;"
            "   color: #00f3ff;"
            "   border: 1px solid #00f3ff;"
            "   border-radius: 5px;"
            "   padding: 6px 16px;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background-color: #00f3ff;"
            "   color: #0b132b;"
            "}"
            );
    }

private slots:
    void onAccept() {
        if (m_comp) {
            m_comp->setId(editId->text().trimmed());
            m_comp->setValue(spinValue->value());
        }
        accept();
    }
};

#endif // COMPONENTPROPERTIESDIALOG_H