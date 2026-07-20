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
    mainLayout->setContentsMargins(45, 40, 45, 40);
    mainLayout->setSpacing(20);

    // ۱. عنوان برنامه
    titleLabel = new QLabel(" Prometheus", this);
    titleLabel->setAlignment(Qt::AlignLeft);
    titleLabel->setStyleSheet(
        "font-size: 30px;"
        "font-weight: bold;"
        "color: #15EFE4;"
        "background: transparent;"
        );
    mainLayout->addWidget(titleLabel, 0, Qt::AlignLeft);

    // ۲. کادر اصلی پروژه‌ها
    QWidget *centerBox = new QWidget(this);
    centerBox->setObjectName("centerBox");
    centerBox->setMaximumWidth(700);

    QHBoxLayout *contentLayout = new QHBoxLayout(centerBox);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(25);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(15);

    QLabel *startTitle = new QLabel("Get Started", centerBox);
    startTitle->setStyleSheet("font-size: 17px; font-weight: bold; background: transparent;");
    leftLayout->addWidget(startTitle);

    newProjectBtn = new QPushButton("Create New Project", centerBox);
    newProjectBtn->setMinimumHeight(45);
    newProjectBtn->setStyleSheet(
        "QPushButton{"
        "background: #4CAF50;"
        "color: white;"
        "border-radius: 8px;"
        "font-size: 14px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover{ background: #66BB6A; }"
        );
    connect(newProjectBtn, &QPushButton::clicked, this, &StartMenu::onNewProjectClicked);
    leftLayout->addWidget(newProjectBtn);

    openProjectBtn = new QPushButton("Open Existing Project", centerBox);
    openProjectBtn->setMinimumHeight(45);
    openProjectBtn->setStyleSheet(
        "QPushButton{"
        "background: #0288D1;"
        "color: white;"
        "border-radius: 8px;"
        "font-size: 14px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover{ background: #03A9F4; }"
        );
    connect(openProjectBtn, &QPushButton::clicked, this, &StartMenu::openProjectRequested);
    leftLayout->addWidget(openProjectBtn);
    leftLayout->addStretch();

    contentLayout->addLayout(leftLayout, 1);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    QLabel *recentTitle = new QLabel("Recent Projects", centerBox);
    recentTitle->setStyleSheet("font-size: 17px; font-weight: bold; background: transparent;");
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

    themeLabel = new QLabel("Theme:", this);
    themeLabel->setStyleSheet("font-size: 14px; font-weight: bold; background: transparent;");

    themeComboBox = new QComboBox(this);
    themeComboBox->addItems(QStringList() << "Dark Mode" << "Ocean Blue");
    themeComboBox->setMinimumWidth(160);
    themeComboBox->setMinimumHeight(32);
    connect(themeComboBox, &QComboBox::currentTextChanged, this, &StartMenu::onThemeChanged);

    settingsLayout->addWidget(themeLabel);
    settingsLayout->addSpacing(15);
    settingsLayout->addWidget(themeComboBox);
    settingsLayout->addStretch();

    mainLayout->addWidget(settingsGroup, 0, Qt::AlignLeft);

    // ۴. 🌟 کادر موزیک و ولوم (تنها کنترلهای فعال پخش صدا)
    QGroupBox *musicGroup = new QGroupBox("🎵 Lofi Player & Audio Control", this);
    musicGroup->setMaximumWidth(700);
    QHBoxLayout *musicLayout = new QHBoxLayout(musicGroup);
    musicLayout->setContentsMargins(15, 12, 15, 12);

    QLabel *musicLabel = new QLabel("Track:", this);
    musicLabel->setStyleSheet("font-size: 14px; font-weight: bold; background: transparent;");

    QComboBox *startMusicCombo = new QComboBox(this);
    startMusicCombo->addItem("Voss", "voss");
    startMusicCombo->addItem("Dream Odyssey", "dream_odyssey");
    startMusicCombo->setMinimumWidth(160);
    startMusicCombo->setMinimumHeight(32);

    QLabel *volLabel = new QLabel("🔊 Volume:", this);
    volLabel->setStyleSheet("font-size: 14px; font-weight: bold; background: transparent;");

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

    // ارسال سیگنال‌ها به MainWindow
    connect(startMusicCombo, &QComboBox::activated, this, [this, startMusicCombo](int index) {
        emit musicChanged(startMusicCombo->itemData(index).toString());
    });

    connect(startVolSlider, &QSlider::valueChanged, this, [this](int value) {
        emit volumeChanged(value);
    });

    mainLayout->addWidget(musicGroup, 0, Qt::AlignLeft);

    // ۵. 🌟 دکمه تعاملی درباره ما (About Us)
    QGroupBox *aboutGroup = new QGroupBox("ℹ Information", this);
    aboutGroup->setMaximumWidth(700);
    QHBoxLayout *aboutLayout = new QHBoxLayout(aboutGroup);
    aboutLayout->setContentsMargins(15, 12, 15, 12);

    QPushButton *btnAboutUs = new QPushButton("📖 About Us", this);
    btnAboutUs->setCursor(Qt::PointingHandCursor);
    btnAboutUs->setMinimumHeight(38);
    btnAboutUs->setStyleSheet(
        "QPushButton {"
        "   background-color: #2c3e50;"
        "   color: #00ff88;"
        "   border: 1px solid #00ff88;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   padding: 0 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #00ff88;"
        "   color: #121212;"
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

// 🌟 باز شدن کادر تعاملی درباره ما
void StartMenu::onAboutUsClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("About Us - Devalopers");
    dialog.setMinimumWidth(380);
    dialog.setStyleSheet(
        "QDialog { background-color: #1a1a1a; border: 1px solid #00ff88; border-radius: 10px; }"
        "QLabel { color: #ffffff; font-size: 14px; font-family: 'Segoe UI'; }"
        "QPushButton { background-color: #00ff88; color: #121212; font-weight: bold; border-radius: 5px; padding: 6px 15px; }"
        "QPushButton:hover { background-color: #27ae60; color: white; }"
        );

    QVBoxLayout layout(&dialog);
    layout.setContentsMargins(20, 20, 20, 20);
    layout.setSpacing(15);

    // ✨ جایگاه قرارگیری اطلاعات شما
    QLabel *lblText = new QLabel("Mahdi Zekri \n" "AmirAli HakimElahi\n" "EE DEP SUT - summer 2026", &dialog);
    lblText->setWordWrap(true);
    lblText->setAlignment(Qt::AlignCenter);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, &dialog);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    layout.addWidget(lblText);
    layout.addWidget(&buttonBox, 0, Qt::AlignCenter);

    dialog.exec();
}

void StartMenu::onThemeChanged(const QString &theme)
{
    if (theme == "Dark Mode") applyDarkTheme();
    else applyBlueTheme();
}

void StartMenu::applyDarkTheme()
{
    setStyleSheet(
        "StartMenu { background: transparent; }"
        "QWidget#centerBox {"
        "   background: rgba(32, 33, 36, 220);"
        "   border: 1px solid #444;"
        "   border-radius: 14px;"
        "   color: #eeeeee;"
        "}"
        "QLabel { color: #eeeeee; }"
        "QGroupBox{"
        "   background: rgba(32, 33, 36, 220);"
        "   border: 1px solid #555;"
        "   border-radius: 12px;"
        "   color: #00ff88;"
        "   font-weight: bold;"
        "   margin-top: 6px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 10px;"
        "   padding: 0 5px;"
        "}"
        "QListWidget{"
        "   background: #151515;"
        "   color: white;"
        "   border-radius: 8px;"
        "   padding: 5px;"
        "}"
        "QComboBox{"
        "   background: #303134;"
        "   color: white;"
        "   border-radius: 6px;"
        "   padding: 4px 8px;"
        "}"
        "QSlider::groove:horizontal {"
        "   height: 5px;"
        "   background: #444;"
        "   border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #00ff88;"
        "   width: 14px;"
        "   height: 14px;"
        "   margin: -5px 0;"
        "   border-radius: 7px;"
        "}"
        );
}

void StartMenu::applyBlueTheme()
{
    setStyleSheet(
        "StartMenu { background: transparent; }"
        "QWidget#centerBox {"
        "   background: rgba(16, 42, 67, 220);"
        "   border: 1px solid #39739D;"
        "   border-radius: 14px;"
        "   color: white;"
        "}"
        "QLabel { color: white; }"
        "QGroupBox{"
        "   background: rgba(16, 42, 67, 220);"
        "   border: 1px solid #39739D;"
        "   border-radius: 12px;"
        "   color: #38ef7d;"
        "   font-weight: bold;"
        "   margin-top: 6px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 10px;"
        "   padding: 0 5px;"
        "}"
        "QListWidget{"
        "   background: #082032;"
        "   color: white;"
        "   border-radius: 8px;"
        "   padding: 5px;"
        "}"
        "QComboBox{"
        "   background: #163A5F;"
        "   color: white;"
        "   border-radius: 6px;"
        "   padding: 4px 8px;"
        "}"
        "QSlider::groove:horizontal {"
        "   height: 5px;"
        "   background: #112e4a;"
        "   border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #38ef7d;"
        "   width: 14px;"
        "   height: 14px;"
        "   margin: -5px 0;"
        "   border-radius: 7px;"
        "}"
        );
}