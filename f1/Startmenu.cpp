#include "startmenu.h"
#include <QDialog>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

StartMenu::StartMenu(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadRecentProjects();
}

StartMenu::~StartMenu() {}

void StartMenu::setupUI()
{
    setMinimumSize(750, 650);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(45, 35, 45, 35);
    mainLayout->setSpacing(18);

    // ۱. عنوان عنوان برنامه با هاله نئونی فیروزه‌ای
    titleLabel = new QLabel("PROMETHEUS SIMULATOR", this);
    titleLabel->setAlignment(Qt::AlignLeft);
    titleLabel->setStyleSheet(
        "font-size: 32px;"
        "font-weight: 900;"
        "color: #00f3ff;"
        "letter-spacing: 2px;"
        "background: transparent;"
        );
    mainLayout->addWidget(titleLabel, 0, Qt::AlignLeft);

    // ۲. کادر اصلی پروژه‌ها (با استایل شیشه‌ای مات کهکشانی)
    QWidget *centerBox = new QWidget(this);
    centerBox->setObjectName("centerBox");
    centerBox->setMaximumWidth(700);

    QHBoxLayout *contentLayout = new QHBoxLayout(centerBox);
    contentLayout->setContentsMargins(22, 22, 22, 22);
    contentLayout->setSpacing(25);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(15);

    QLabel *startTitle = new QLabel("⚡ Get Started", centerBox);
    startTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #00f3ff; background: transparent;");
    leftLayout->addWidget(startTitle);

    newProjectBtn = new QPushButton("Create New Project", centerBox);
    newProjectBtn->setCursor(Qt::PointingHandCursor);
    newProjectBtn->setMinimumHeight(45);
    newProjectBtn->setStyleSheet(
        "QPushButton {"
        "   background: linear-gradient(135deg, #0d6efd, #00d2ff);"
        "   color: white;"
        "   border: 1px solid #00f3ff;"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: linear-gradient(135deg, #00d2ff, #00ff88);"
        "   color: #0d6efd;"
        "   border: 1px solid #00ff88;"
        "}"
        );
    connect(newProjectBtn, &QPushButton::clicked, this, &StartMenu::onNewProjectClicked);
    leftLayout->addWidget(newProjectBtn);

    openProjectBtn = new QPushButton("Open Existing Project", centerBox);
    openProjectBtn->setCursor(Qt::PointingHandCursor);
    openProjectBtn->setMinimumHeight(45);
    openProjectBtn->setStyleSheet(
        "QPushButton {"
        "   background: rgba(15, 23, 42, 180);"
        "   color: #00f3ff;"
        "   border: 1px solid rgba(0, 243, 255, 0.5);"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(0, 243, 255, 0.2);"
        "   border: 1px solid #00f3ff;"
        "   color: #ffffff;"
        "}"
        );
    connect(openProjectBtn, &QPushButton::clicked, this, &StartMenu::openProjectRequested);
    leftLayout->addWidget(openProjectBtn);
    leftLayout->addStretch();

    contentLayout->addLayout(leftLayout, 1);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    QLabel *recentTitle = new QLabel("🕒 Recent Projects", centerBox);
    recentTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #00f3ff; background: transparent;");
    rightLayout->addWidget(recentTitle);

    recentProjectsList = new QListWidget(centerBox);
    recentProjectsList->setMinimumHeight(160);
    connect(recentProjectsList, &QListWidget::currentTextChanged, this, &StartMenu::recentProjectSelected);
    rightLayout->addWidget(recentProjectsList);

    contentLayout->addLayout(rightLayout, 1);
    mainLayout->addWidget(centerBox, 0, Qt::AlignLeft);

    // ۳. کادر تنظیمات تم (Settings)
    settingsGroup = new QGroupBox("⚙ Settings", this);
    settingsGroup->setMaximumWidth(700);
    QHBoxLayout *settingsLayout = new QHBoxLayout(settingsGroup);
    settingsLayout->setContentsMargins(15, 12, 15, 12);

    themeLabel = new QLabel("Theme Profile:", this);
    themeLabel->setStyleSheet("font-size: 13px; font-weight: bold; background: transparent;");

    // 🌟 تغییر نام گزینه‌های تم به Eclipse و Genesis
    themeComboBox = new QComboBox(this);
    themeComboBox->addItems(QStringList() << "Eclipse" << "Genesis");
    themeComboBox->setMinimumWidth(160);
    themeComboBox->setMinimumHeight(32);
    connect(themeComboBox, &QComboBox::currentTextChanged, this, &StartMenu::onThemeChanged);

    settingsLayout->addWidget(themeLabel);
    settingsLayout->addSpacing(15);
    settingsLayout->addWidget(themeComboBox);
    settingsLayout->addStretch();

    mainLayout->addWidget(settingsGroup, 0, Qt::AlignLeft);

    // ۴. 🌟 کادر موزیک و ولوم
    QGroupBox *musicGroup = new QGroupBox("🎵 Lofi Player & Audio Control", this);
    musicGroup->setMaximumWidth(700);
    QHBoxLayout *musicLayout = new QHBoxLayout(musicGroup);
    musicLayout->setContentsMargins(15, 12, 15, 12);

    QLabel *musicLabel = new QLabel("Track:", this);
    musicLabel->setStyleSheet("font-size: 13px; font-weight: bold; background: transparent;");

    QComboBox *startMusicCombo = new QComboBox(this);
    startMusicCombo->addItem("Voss", "voss");
    startMusicCombo->addItem("Dream Odyssey", "dream_odyssey");
    startMusicCombo->setMinimumWidth(160);
    startMusicCombo->setMinimumHeight(32);

    QLabel *volLabel = new QLabel("🔊 Volume:", this);
    volLabel->setStyleSheet("font-size: 13px; font-weight: bold; background: transparent;");

    QSlider *startVolSlider = new QSlider(Qt::Horizontal, this);
    startVolSlider->setRange(0, 100);
    startVolSlider->setValue(70);
    startVolSlider->setMinimumWidth(160);

    musicLayout->addWidget(musicLabel);
    musicLayout->addWidget(startMusicCombo);
    musicLayout->addSpacing(25);
    musicLayout->addWidget(volLabel);
    musicLayout->addWidget(startVolSlider);
    musicLayout->addStretch();

    connect(startMusicCombo, &QComboBox::activated, this, [this, startMusicCombo](int index) {
        emit musicChanged(startMusicCombo->itemData(index).toString());
    });

    connect(startVolSlider, &QSlider::valueChanged, this, [this](int value) {
        emit volumeChanged(value);
    });

    mainLayout->addWidget(musicGroup, 0, Qt::AlignLeft);

    // ۵. 🌟 دکمه تعاملی درباره ما (Information)
    QGroupBox *aboutGroup = new QGroupBox("ℹ System Info", this);
    aboutGroup->setMaximumWidth(700);
    QHBoxLayout *aboutLayout = new QHBoxLayout(aboutGroup);
    aboutLayout->setContentsMargins(15, 12, 15, 12);

    QPushButton *btnAboutUs = new QPushButton("📖 Developers Info", this);
    btnAboutUs->setCursor(Qt::PointingHandCursor);
    btnAboutUs->setMinimumHeight(36);
    btnAboutUs->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(0, 243, 255, 0.1);"
        "   color: #00f3ff;"
        "   border: 1px solid #00f3ff;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   padding: 0 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #00f3ff;"
        "   color: #050b14;"
        "}"
        );
    connect(btnAboutUs, &QPushButton::clicked, this, &StartMenu::onAboutUsClicked);

    aboutLayout->addWidget(btnAboutUs);
    aboutLayout->addStretch();

    mainLayout->addWidget(aboutGroup, 0, Qt::AlignLeft);
    mainLayout->addStretch();

    applyDarkTheme();
}

void StartMenu::loadRecentProjects()
{
    recentProjectsList->addItem("Digital Clock Project.json");
    recentProjectsList->addItem("Counter 8bit Project.json");
    recentProjectsList->addItem("Power Supply Project.json");
}

void StartMenu::onNewProjectClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Create New Project");
    QVBoxLayout layout(&dialog);

    QLabel label("Choose Canvas Size:", &dialog);
    layout.addWidget(&label);

    QRadioButton a4("A4 Standard", &dialog);
    QRadioButton a3("A3 Large", &dialog);
    QRadioButton custom("Custom 1000x1000", &dialog);
    a4.setChecked(true);

    layout.addWidget(&a4);
    layout.addWidget(&a3);
    layout.addWidget(&custom);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout.addWidget(&buttons);

    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString size = a4.isChecked() ? "A4" : (a3.isChecked() ? "A3" : "Custom");
        emit newProjectRequested(size);
    }
}

void StartMenu::onAboutUsClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("About Us - Developers");
    dialog.setMinimumWidth(380);
    dialog.setStyleSheet(
        "QDialog { background-color: #0b0f19; border: 1px solid #00f3ff; border-radius: 12px; }"
        "QLabel { color: #e2e8f0; font-size: 14px; font-family: 'Segoe UI'; }"
        "QPushButton { background-color: #00f3ff; color: #0b0f19; font-weight: bold; border-radius: 6px; padding: 6px 18px; }"
        "QPushButton:hover { background-color: #00ff88; color: #050b14; }"
        );

    QVBoxLayout layout(&dialog);
    layout.setContentsMargins(22, 22, 22, 22);
    layout.setSpacing(15);

    QLabel *lblText = new QLabel("Mahdi Zekri \nAmirAli HakimElahi\nEE DEP SUT - summer 2026", &dialog);
    lblText->setWordWrap(true);
    lblText->setAlignment(Qt::AlignCenter);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, &dialog);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    layout.addWidget(lblText);
    layout.addWidget(&buttonBox, 0, Qt::AlignCenter);

    dialog.exec();
}

// 🌟 به‌روزرسانی شرط تعویض تم براساس گزینه‌های Eclipse و Genesis
void StartMenu::onThemeChanged(const QString &theme)
{
    if (theme == "Eclipse") applyDarkTheme();
    else applyBlueTheme();
}

// 🌟 تم Eclipse (تاریک کهکشانی + پس‌زمینه ANNA.jpg)
void StartMenu::applyDarkTheme()
{
    if (this->window()) {
        this->window()->setStyleSheet(
            "QMainWindow, #MainWindow {"
            "   border-image: url(':/image/ANNA.jpg') 0 0 0 0 stretch stretch;"
            "   background-position: center;"
            "}"
            );
    }

    setStyleSheet(
        "StartMenu { background: transparent; }"
        "QWidget#centerBox {"
        "   background: rgba(8, 14, 28, 200);"
        "   border: 1px solid rgba(0, 243, 255, 0.4);"
        "   border-radius: 16px;"
        "   color: #f1f5f9;"
        "}"
        "QLabel { color: #f1f5f9; }"
        "QGroupBox {"
        "   background: rgba(8, 14, 28, 190);"
        "   border: 1px solid rgba(0, 243, 255, 0.3);"
        "   border-radius: 12px;"
        "   color: #00f3ff;"
        "   font-weight: bold;"
        "   margin-top: 6px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 12px;"
        "   padding: 0 6px;"
        "}"
        "QListWidget {"
        "   background: rgba(5, 10, 20, 220);"
        "   color: #00f3ff;"
        "   border: 1px solid rgba(0, 243, 255, 0.2);"
        "   border-radius: 8px;"
        "   padding: 5px;"
        "}"
        "QListWidget::item:hover {"
        "   background: rgba(0, 243, 255, 0.15);"
        "   border-radius: 4px;"
        "}"
        "QComboBox {"
        "   background: #0f172a;"
        "   color: #00f3ff;"
        "   border: 1px solid rgba(0, 243, 255, 0.4);"
        "   border-radius: 6px;"
        "   padding: 4px 8px;"
        "}"
        "QSlider::groove:horizontal {"
        "   height: 5px;"
        "   background: #1e293b;"
        "   border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #00f3ff;"
        "   width: 14px;"
        "   height: 14px;"
        "   margin: -5px 0;"
        "   border-radius: 7px;"
        "}"
        );
}

// 🌟 تم Genesis (آبی-اقیانوسی + پس‌زمینه ANNA2.jpg)
// 🌟 تم Genesis (هماهنگ‌شده با تم سنگی و نئون‌های طلایی ANNA2.jpg)
void StartMenu::applyBlueTheme()
{
    if (this->window()) {
        this->window()->setStyleSheet(
            "QMainWindow, #MainWindow {"
            "   border-image: url(':/image/ANNA2.jpg') 0 0 0 0 stretch stretch;"
            "   background-position: center;"
            "}"
            );
    }

    setStyleSheet(
        "StartMenu { background: transparent; }"
        "QWidget#centerBox {"
        "   background: rgba(12, 16, 23, 215);"
        "   border: 1px solid rgba(229, 169, 59, 0.5);"
        "   border-radius: 16px;"
        "   color: #ffffff;"
        "}"
        "QLabel { color: #f0e2c6; }"
        "QGroupBox {"
        "   background: rgba(12, 16, 23, 195);"
        "   border: 1px solid rgba(229, 169, 59, 0.35);"
        "   border-radius: 12px;"
        "   color: #e5a93b;"
        "   font-weight: bold;"
        "   margin-top: 6px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 12px;"
        "   padding: 0 6px;"
        "}"
        "QListWidget {"
        "   background: rgba(8, 11, 16, 230);"
        "   color: #e5a93b;"
        "   border: 1px solid rgba(229, 169, 59, 0.25);"
        "   border-radius: 8px;"
        "   padding: 5px;"
        "}"
        "QListWidget::item:hover {"
        "   background: rgba(229, 169, 59, 0.15);"
        "   border-radius: 4px;"
        "}"
        "QComboBox {"
        "   background: #111620;"
        "   color: #e5a93b;"
        "   border: 1px solid rgba(229, 169, 59, 0.4);"
        "   border-radius: 6px;"
        "   padding: 4px 8px;"
        "}"
        "QSlider::groove:horizontal {"
        "   height: 5px;"
        "   background: #18202c;"
        "   border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #e5a93b;"
        "   width: 14px;"
        "   height: 14px;"
        "   margin: -5px 0;"
        "   border-radius: 7px;"
        "}"
        );
}