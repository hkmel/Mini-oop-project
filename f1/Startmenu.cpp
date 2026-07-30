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
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QSettings>
#include <QFileInfo>

StartMenu::StartMenu(QWidget *parent)
    : QWidget(parent)
    , currentTheme("Eclipse")
    , currentTrack("voss")
    , currentVolume(70)
{
    setupUI();
    loadRecentProjects();
}

StartMenu::~StartMenu() {}

void StartMenu::setupUI()
{
    setMinimumSize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(45, 30, 45, 30);
    mainLayout->setSpacing(20);

    // -----------------------------------------------------------------
    // هدر بالا: عنوان + نشانگر وضعیت موتور + دکمه تنظیمات
    // -----------------------------------------------------------------
    QHBoxLayout *headerLayout = new QHBoxLayout();

    titleLabel = new QLabel("PROMETHEUS SIMULATOR", this);
    titleLabel->setStyleSheet(
        "font-size: 30px;"
        "font-weight: 900;"
        "color: #00f3ff;"
        "letter-spacing: 2px;"
        "background: transparent;"
        );

    settingsBtn = new QPushButton("⚙ Preferences", this);
    settingsBtn->setCursor(Qt::PointingHandCursor);
    settingsBtn->setMinimumHeight(38);
    settingsBtn->setStyleSheet(
        "QPushButton {"
        "   background: rgba(15, 23, 42, 200);"
        "   color: #00f3ff;"
        "   border: 1px solid rgba(0, 243, 255, 0.5);"
        "   border-radius: 8px;"
        "   font-weight: bold;"
        "   padding: 0 16px;"
        "}"
        "QPushButton:hover {"
        "   background: #00f3ff;"
        "   color: #0b0f19;"
        "}"
        );
    connect(settingsBtn, &QPushButton::clicked, this, &StartMenu::openSettingsDialog);

    headerLayout->addWidget(titleLabel);
    headerLayout->addSpacing(10);
    headerLayout->addWidget(statusBadge);
    headerLayout->addStretch();
    headerLayout->addWidget(settingsBtn);

    mainLayout->addLayout(headerLayout);

    // -----------------------------------------------------------------
    // کادر مرکزی دو ستونه (شروع سریع + پروژه‌های اخیر)
    // -----------------------------------------------------------------
    centerBox = new QWidget(this);
    centerBox->setObjectName("centerBox");

    QHBoxLayout *contentLayout = new QHBoxLayout(centerBox);
    contentLayout->setContentsMargins(25, 25, 25, 25);
    contentLayout->setSpacing(30);

    // ستون چپ: اکشن‌های اصلی
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(16);

    QLabel *startTitle = new QLabel("🚀 Launch Action", centerBox);
    startTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #00f3ff; background: transparent;");
    leftLayout->addWidget(startTitle);

    newProjectBtn = new QPushButton("Create New Project", centerBox);
    newProjectBtn->setCursor(Qt::PointingHandCursor);
    newProjectBtn->setMinimumHeight(48);
    newProjectBtn->setStyleSheet(
        "QPushButton {"
        "   background: linear-gradient(135deg, #0d6efd, #00d2ff);"
        "   color: white;"
        "   border: 1px solid #00f3ff;"
        "   border-radius: 10px;"
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
    openProjectBtn->setMinimumHeight(48);
    openProjectBtn->setStyleSheet(
        "QPushButton {"
        "   background: rgba(15, 23, 42, 180);"
        "   color: #00f3ff;"
        "   border: 1px solid rgba(0, 243, 255, 0.5);"
        "   border-radius: 10px;"
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

    // راهنمای سریع کلیدهای میانبر
    QLabel *shortcutInfo = new QLabel("💡 Quick Tips:\n• Make and Test!\n• Press 'ctrl+R' to Rotate Component\n• 'turn off your VPN to best performance'\n•'version 1.1' ", centerBox);
    shortcutInfo->setStyleSheet("color: #94a3b8; font-size: 12px; line-height: 1.4; margin-top: 10px; background: transparent;");
    leftLayout->addWidget(shortcutInfo);

    leftLayout->addStretch();
    contentLayout->addLayout(leftLayout, 1);

    // ستون راست: پروژه‌های اخیر و فیلتر جستجو
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(10);

    QLabel *recentTitle = new QLabel("🕒 Recent Projects", centerBox);
    recentTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #00f3ff; background: transparent;");
    rightLayout->addWidget(recentTitle);

    searchEdit = new QLineEdit(centerBox);
    searchEdit->setPlaceholderText("🔍 Search recent...");
    searchEdit->setStyleSheet(
        "QLineEdit {"
        "   background: rgba(5, 10, 20, 200);"
        "   color: #00f3ff;"
        "   border: 1px solid rgba(0, 243, 255, 0.3);"
        "   border-radius: 6px;"
        "   padding: 6px 10px;"
        "   font-size: 12px;"
        "}"
        "QLineEdit:focus { border: 1px solid #00f3ff; }"
        );
    connect(searchEdit, &QLineEdit::textChanged, this, &StartMenu::filterRecentProjects);
    rightLayout->addWidget(searchEdit);

    recentProjectsList = new QListWidget(centerBox);
    recentProjectsList->setMinimumHeight(200);
    connect(recentProjectsList, &QListWidget::itemClicked, this, &StartMenu::onRecentItemClicked);
    rightLayout->addWidget(recentProjectsList);

    contentLayout->addLayout(rightLayout, 1);
    mainLayout->addWidget(centerBox);

    // -----------------------------------------------------------------
    // فوتر پایین: اطلاعات سازندگان
    // -----------------------------------------------------------------
    QHBoxLayout *footerLayout = new QHBoxLayout();

    aboutUsBtn = new QPushButton("📖 Developers Info", this);
    aboutUsBtn->setCursor(Qt::PointingHandCursor);
    aboutUsBtn->setMinimumHeight(32);
    aboutUsBtn->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #94a3b8;"
        "   border: none;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   color: #00f3ff;"
        "   text-decoration: underline;"
        "}"
        );
    connect(aboutUsBtn, &QPushButton::clicked, this, &StartMenu::onAboutUsClicked);

    footerLayout->addWidget(aboutUsBtn);
    footerLayout->addStretch();

    mainLayout->addLayout(footerLayout);

    applyDarkTheme();
}

// ---------------------------------------------------------------------
// پنجره یکپارچه تنظیمات (Preferences Modal)
// ---------------------------------------------------------------------
void StartMenu::openSettingsDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("⚙ Preferences & Settings");
    dialog.setMinimumWidth(440);
    dialog.setStyleSheet(
        "QDialog { background-color: #0b0f19; border: 1px solid #00f3ff; border-radius: 12px; }"
        "QLabel { color: #e2e8f0; font-size: 13px; font-weight: bold; font-family: 'Segoe UI'; }"
        "QGroupBox {"
        "   background: rgba(15, 23, 42, 180);"
        "   border: 1px solid rgba(0, 243, 255, 0.3);"
        "   border-radius: 8px;"
        "   color: #00f3ff;"
        "   font-weight: bold;"
        "   margin-top: 10px;"
        "   padding-top: 12px;"
        "}"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }"
        "QComboBox {"
        "   background: #0f172a;"
        "   color: #00f3ff;"
        "   border: 1px solid rgba(0, 243, 255, 0.4);"
        "   border-radius: 6px;"
        "   padding: 5px 8px;"
        "}"
        "QSlider::groove:horizontal { height: 6px; background: #1e293b; border-radius: 3px; }"
        "QSlider::handle:horizontal { background: #00f3ff; width: 16px; height: 16px; margin: -5px 0; border-radius: 8px; }"
        );

    QVBoxLayout layout(&dialog);
    layout.setContentsMargins(20, 20, 20, 20);
    layout.setSpacing(15);

    // ۱. گروه تم‌ها
    QGroupBox *themeGroup = new QGroupBox("🎨 Theme Profile", &dialog);
    QHBoxLayout *themeLayout = new QHBoxLayout(themeGroup);
    QComboBox *dlgThemeCombo = new QComboBox(&dialog);
    dlgThemeCombo->addItems(QStringList() << "Eclipse" << "Genesis");
    dlgThemeCombo->setCurrentText(currentTheme);
    themeLayout->addWidget(new QLabel("Visual Preset:", &dialog));
    themeLayout->addWidget(dlgThemeCombo);

    connect(dlgThemeCombo, &QComboBox::currentTextChanged, this, [this](const QString &theme) {
        currentTheme = theme;
        onThemeChanged(theme);
    });

    layout.addWidget(themeGroup);

    // ۲. گروه پخش موزیک و ولوم
    QGroupBox *audioGroup = new QGroupBox("🎵 Audio & Lo-Fi Player", &dialog);
    QVBoxLayout *audioLayout = new QVBoxLayout(audioGroup);
    audioLayout->setSpacing(12);

    QHBoxLayout *trackLayout = new QHBoxLayout();
    QComboBox *dlgTrackCombo = new QComboBox(&dialog);
    dlgTrackCombo->addItem("Voss", "voss");
    dlgTrackCombo->addItem("Dream Odyssey", "dream_odyssey");
    int trackIdx = dlgTrackCombo->findData(currentTrack);
    if (trackIdx != -1) dlgTrackCombo->setCurrentIndex(trackIdx);

    trackLayout->addWidget(new QLabel("Track:", &dialog));
    trackLayout->addWidget(dlgTrackCombo);
    audioLayout->addLayout(trackLayout);

    connect(dlgTrackCombo, &QComboBox::activated, this, [this, dlgTrackCombo](int index) {
        currentTrack = dlgTrackCombo->itemData(index).toString();
        emit musicChanged(currentTrack);
    });

    QHBoxLayout *volLayout = new QHBoxLayout();
    QSlider *dlgVolSlider = new QSlider(Qt::Horizontal, &dialog);
    dlgVolSlider->setRange(0, 100);
    dlgVolSlider->setValue(currentVolume);

    QLabel *volValLabel = new QLabel(QString::number(currentVolume) + "%", &dialog);
    volValLabel->setMinimumWidth(35);

    volLayout->addWidget(new QLabel("🔊 Volume:", &dialog));
    volLayout->addWidget(dlgVolSlider);
    volLayout->addWidget(volValLabel);
    audioLayout->addLayout(volLayout);

    connect(dlgVolSlider, &QSlider::valueChanged, this, [this, volValLabel](int value) {
        currentVolume = value;
        volValLabel->setText(QString::number(value) + "%");
        emit volumeChanged(value);
    });

    layout.addWidget(audioGroup);

    // ۳. دکمه تایید
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, &dialog);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    layout.addWidget(&buttonBox, 0, Qt::AlignRight);

    dialog.exec();
}

void StartMenu::filterRecentProjects(const QString &text)
{
    for (int i = 0; i < recentProjectsList->count(); ++i) {
        QListWidgetItem *item = recentProjectsList->item(i);
        bool matches = item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!matches);
    }
}

void StartMenu::loadRecentProjects()
{
    recentProjectsList->clear();
    QSettings settings("PrometheusTeam", "PrometheusSimulator");
    QStringList recentFiles = settings.value("recentProjects").toStringList();

    for (const QString &filePath : recentFiles) {
        QFileInfo fileInfo(filePath);
        QListWidgetItem *item = new QListWidgetItem(fileInfo.fileName(), recentProjectsList);
        item->setData(Qt::UserRole, filePath);
        item->setToolTip(filePath);
    }
}

void StartMenu::addRecentProject(const QString &filePath)
{
    if (filePath.isEmpty()) return;

    QSettings settings("PrometheusTeam", "PrometheusSimulator");
    QStringList recentFiles = settings.value("recentProjects").toStringList();

    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);

    while (recentFiles.size() > 10) {
        recentFiles.removeLast();
    }

    settings.setValue("recentProjects", recentFiles);
    loadRecentProjects();
}

void StartMenu::onRecentItemClicked(QListWidgetItem *item)
{
    if (!item) return;
    QString fullPath = item->data(Qt::UserRole).toString();
    if (fullPath.isEmpty()) fullPath = item->text();

    emit recentProjectSelected(fullPath);
}

void StartMenu::onNewProjectClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Create New Project");
    dialog.setMinimumWidth(420);
    dialog.setStyleSheet(
        "QDialog { background-color: #0b0f19; border: 1px solid #00f3ff; border-radius: 12px; }"
        "QLabel { color: #e2e8f0; font-size: 13px; font-weight: bold; font-family: 'Segoe UI'; }"
        "QLineEdit { background-color: #0f172a; color: #00f3ff; border: 1px solid rgba(0, 243, 255, 0.4); border-radius: 6px; padding: 6px; font-size: 13px; font-weight: bold; }"
        "QLineEdit:focus { border: 1px solid #00f3ff; }"
        "QPushButton#btnBrowse { background-color: rgba(0, 243, 255, 0.15); color: #00f3ff; border: 1px solid #00f3ff; border-radius: 6px; padding: 6px 12px; font-weight: bold; }"
        "QPushButton#btnBrowse:hover { background-color: #00f3ff; color: #0b0f19; }"
        "QRadioButton { color: #e2e8f0; font-size: 13px; font-family: 'Segoe UI'; }"
        );

    QVBoxLayout layout(&dialog);
    layout.setContentsMargins(20, 20, 20, 20);
    layout.setSpacing(12);

    QLabel *nameLabel = new QLabel("Project Name:", &dialog);
    QLineEdit *nameEdit = new QLineEdit("Untitled_Project", &dialog);
    layout.addWidget(nameLabel);
    layout.addWidget(nameEdit);

    QLabel *pathLabel = new QLabel("Save Location:", &dialog);
    QHBoxLayout *pathLayout = new QHBoxLayout();
    QLineEdit *pathEdit = new QLineEdit(QDir::homePath() + "/Desktop", &dialog);
    QPushButton *browseBtn = new QPushButton("Browse...", &dialog);
    browseBtn->setObjectName("btnBrowse");
    browseBtn->setCursor(Qt::PointingHandCursor);

    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(browseBtn);

    layout.addWidget(pathLabel);
    layout.addLayout(pathLayout);

    connect(browseBtn, &QPushButton::clicked, &dialog, [&dialog, pathEdit]() {
        QString dir = QFileDialog::getExistingDirectory(&dialog, "Select Save Directory", pathEdit->text());
        if (!dir.isEmpty()) {
            pathEdit->setText(dir);
        }
    });

    QLabel *label = new QLabel("Choose Canvas Size:", &dialog);
    layout.addWidget(label);

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

        QString projectName = nameEdit->text().trimmed();
        if (projectName.isEmpty()) projectName = "Untitled_Project";
        if (!projectName.endsWith(".json", Qt::CaseInsensitive)) {
            projectName += ".json";
        }
        QString savePath = pathEdit->text();
        QString fullPath = QDir(savePath).filePath(projectName);

        addRecentProject(fullPath);

        emit newProjectRequested(size, projectName, savePath);
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

void StartMenu::onThemeChanged(const QString &theme)
{
    if (theme == "Eclipse") applyDarkTheme();
    else applyBlueTheme();
}

void StartMenu::applyDarkTheme()
{
    if (statusBadge) statusBadge->setStyleSheet("background: rgba(0, 243, 255, 0.1); color: #00f3ff; border: 1px solid rgba(0, 243, 255, 0.4); border-radius: 12px; padding: 4px 10px; font-size: 11px; font-weight: bold;");

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
        );
}

void StartMenu::applyBlueTheme()
{
    if (statusBadge) statusBadge->setStyleSheet("background: rgba(229, 169, 59, 0.1); color: #e5a93b; border: 1px solid rgba(229, 169, 59, 0.4); border-radius: 12px; padding: 4px 10px; font-size: 11px; font-weight: bold;");

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
        );
}