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
#include <QGraphicsDropShadowEffect>
#include <QFileInfo>
#include <QTextBrowser>
#include <QScrollBar>

StartMenu::StartMenu(QWidget *parent)
    : QWidget(parent)
    , statusBadge(nullptr)
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
    setMinimumSize(850, 620);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 25, 40, 25);
    mainLayout->setSpacing(20);

    // -----------------------------------------------------------------
    // ۱. هدر بالا: عنوان + نشانگر وضعیت موتور + دکمه تنظیمات
    // -----------------------------------------------------------------
    QHBoxLayout *headerLayout = new QHBoxLayout();

    titleLabel = new QLabel("PROMETHEUS SIMULATOR", this);
    titleLabel->setStyleSheet(
        "font-size: 28px;"
        "font-weight: 900;"
        "color: #00f3ff;"
        "letter-spacing: 2px;"
        "background: transparent;"
        );

    statusBadge = new QLabel("● ENGINE ONLINE", this);
    statusBadge->setStyleSheet(
        "background: rgba(0, 243, 255, 0.1);"
        "color: #00f3ff;"
        "border: 1px solid rgba(0, 243, 255, 0.4);"
        "border-radius: 12px;"
        "padding: 4px 10px;"
        "font-size: 11px;"
        "font-weight: bold;"
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
    // ۲. کادر مرکزی دو ستونه (شروع سریع + پروژه‌های اخیر)
    // -----------------------------------------------------------------
    centerBox = new QWidget(this);
    centerBox->setObjectName("centerBox");

    QHBoxLayout *contentLayout = new QHBoxLayout(centerBox);
    contentLayout->setContentsMargins(25, 25, 25, 25);
    contentLayout->setSpacing(30);

    // ---------------- ستون چپ: اکشن‌های اصلی ----------------
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(14);

    QLabel *startTitle = new QLabel("🚀 Launch Action", centerBox);
    startTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #00f3ff; background: transparent;");
    leftLayout->addWidget(startTitle);

    newProjectBtn = new QPushButton("Create New Project", centerBox);
    newProjectBtn->setCursor(Qt::PointingHandCursor);
    newProjectBtn->setMinimumHeight(46);
    newProjectBtn->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0d6efd, stop:1 #00d2ff);"
        "   color: white;"
        "   border: 1px solid #00f3ff;"
        "   border-radius: 10px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #00d2ff, stop:1 #00ff88);"
        "   color: #050b14;"
        "   border: 1px solid #00ff88;"
        "}"
        );
    connect(newProjectBtn, &QPushButton::clicked, this, &StartMenu::onNewProjectClicked);
    leftLayout->addWidget(newProjectBtn);

    openProjectBtn = new QPushButton("Open Existing Project", centerBox);
    openProjectBtn->setCursor(Qt::PointingHandCursor);
    openProjectBtn->setMinimumHeight(46);
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

    // --- کادر راهنما و سازندگان (Help & Developers Side-by-Side) ---
    QGroupBox *infoBox = new QGroupBox("Information & Assistance", centerBox);
    infoBox->setStyleSheet(
        "QGroupBox {"
        "   background: rgba(5, 10, 20, 150);"
        "   border: 1px solid rgba(0, 243, 255, 0.3);"
        "   border-radius: 10px;"
        "   color: #94a3b8;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "   margin-top: 10px;"
        "   padding-top: 15px;"
        "}"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }"
        );

    QHBoxLayout *infoLayout = new QHBoxLayout(infoBox);
    infoLayout->setSpacing(10);
    infoLayout->setContentsMargins(10, 12, 10, 12);

    helpBtn = new QPushButton("❓ Help Guide", infoBox);
    helpBtn->setCursor(Qt::PointingHandCursor);
    helpBtn->setMinimumHeight(38);
    helpBtn->setStyleSheet(
        "QPushButton {"
        "   background: rgba(0, 243, 255, 0.12);"
        "   color: #00f3ff;"
        "   border: 1px solid #00f3ff;"
        "   border-radius: 8px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: #00f3ff;"
        "   color: #050b14;"
        "}"
        );
    connect(helpBtn, &QPushButton::clicked, this, &StartMenu::openHelpDialog);

    aboutUsBtn = new QPushButton("📖 Developers Info", infoBox);
    aboutUsBtn->setCursor(Qt::PointingHandCursor);
    aboutUsBtn->setMinimumHeight(38);
    aboutUsBtn->setStyleSheet(
        "QPushButton {"
        "   background: rgba(255, 255, 255, 0.05);"
        "   color: #e2e8f0;"
        "   border: 1px solid rgba(255, 255, 255, 0.2);"
        "   border-radius: 8px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 255, 255, 0.18);"
        "   color: #ffffff;"
        "   border-color: #ffffff;"
        "}"
        );
    connect(aboutUsBtn, &QPushButton::clicked, this, &StartMenu::onAboutUsClicked);

    infoLayout->addWidget(helpBtn);
    infoLayout->addWidget(aboutUsBtn);

    leftLayout->addWidget(infoBox);
    leftLayout->addStretch();

    contentLayout->addLayout(leftLayout, 1);

    // ---------------- ستون راست: پروژه‌های اخیر و فیلتر جستجو ----------------
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
    recentProjectsList->setFixedHeight(180);
    connect(recentProjectsList, &QListWidget::itemClicked, this, &StartMenu::onRecentItemClicked);
    rightLayout->addWidget(recentProjectsList);
    rightLayout->addStretch();

    contentLayout->addLayout(rightLayout, 1);
    mainLayout->addWidget(centerBox);

    applyDarkTheme();
}

// ---------------------------------------------------------------------
// ۳. پنجره اسکرول‌دار راهنمای کامل برنامه (Help Window)
// ---------------------------------------------------------------------
void StartMenu::openHelpDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("📖 Prometheus Simulator - Help & User Guide");
    dialog.resize(620, 520);
    dialog.setStyleSheet(
        "QDialog { background-color: #0b0f19; border: 1px solid #00f3ff; border-radius: 12px; }"
        "QTextBrowser {"
        "   background-color: #080e18;"
        "   color: #e2e8f0;"
        "   border: 1px solid rgba(0, 243, 255, 0.3);"
        "   border-radius: 8px;"
        "   padding: 12px;"
        "   font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;"
        "   font-size: 13px;"
        "}"
        "QScrollBar:vertical {"
        "   border: none;"
        "   background: #050a14;"
        "   width: 10px;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #00f3ff;"
        "   min-height: 20px;"
        "   border-radius: 5px;"
        "}"
        "QPushButton {"
        "   background-color: #00f3ff;"
        "   color: #0b0f19;"
        "   font-weight: bold;"
        "   border-radius: 6px;"
        "   padding: 8px 24px;"
        "}"
        "QPushButton:hover { background-color: #00ff88; }"
        );

    QVBoxLayout layout(&dialog);
    layout.setContentsMargins(20, 20, 20, 20);
    layout.setSpacing(15);

    QTextBrowser *helpView = new QTextBrowser(&dialog);
    helpView->setOpenExternalLinks(true);

    // محتوای راهنما با فرمت HTML و استایل‌دهی شیک
    QString helpContent = R"(
        <h2 style="color: #00f3ff; text-align: center; margin-bottom: 15px;">⚡ راهنمای جامع استفاده از شبیه‌ساز پرومتئوس</h2>
        <hr style="border: 1px solid rgba(0, 243, 255, 0.3);" />

        <h3 style="color: #00ff88;">⌨️ کلیدهای میانبر و کنترل‌های بوم (Canvas Controls)</h3>
        <ul>
            <li><b>چرخش قطعات:</b> برای چرخاندن قطعه انتخاب‌شده، کلید ترکیب <b>Ctrl + R</b> را فشار دهید.</li>
            <li><b>بزرگ‌نمایی (Zoom):</b> با استفاده از <b>اسکرول ماوس (Mouse Wheel)</b> می‌توانید بوم را زوم کنید.</li>
            <li><b>حذف المان‌ها:</b> قطعه یا سیم مورد نظر را انتخاب کرده و کلید <b>Delete</b> یا <b>Backspace</b> را بزنید.</li>
            <li><b>جابجایی بوم:</b> کلید کلیک راست ماوس را نگه داشته و بوم را جابجا کنید.</li>
        </ul>

        <h3 style="color: #00ff88;">💾 ذخیره و بازکردن پروژه‌ها</h3>
        <ul>
            <li><b>ذخیره‌سازی (Save):</b> در نوار ابزار بالای بوم روی دکمه <b>Save</b> کلیک کنید. پروژه شما در قالب فایل استاندارد JSON ذخیره می‌شود.</li>
            <li><b>باز کردن پروژه (Open):</b> از طریق دکمه <b>Open</b> یا لیست <b>Recent Projects</b> در استارت‌منو، فایل‌های قبلی را بارگذاری کنید.</li>
        </ul>

        <h3 style="color: #00ff88;">🧩 کاربرد انواع قطعات (Component Types)</h3>
        <ul>
            <li><b>Power Sources (منابع تغذیه):</b> شامل VCC (ولتاژ مثبت) و GND (زمین) برای تامین تغذیه مدار.</li>
            <li><b>Logic Gates (گیت‌های منطقی):</b> گیت‌های اصلی مانند AND, OR, NOT, NAND, NOR, XOR برای طراحی مدارهای دیجیتال.</li>
            <li><b>Passive Components (قطعات پسیو):</b> مقاومت (Resistor)، خازن (Capacitor) و سلف (Inductor).</li>
            <li><b>Switches & Displays (کلیدها و نمایشگرها):</b> Push Button، کلیدهای حالت‌دار (Toggle Switch)، LED و 7-Segment.</li>
            <li><b>Measurement Instruments (ابزار اندازه‌گیری):</b> ولتمتر (Voltmeter) و آمپرمتر (Ammeter) جهت مشاهده لحظه‌ای ولتاژ و جریان.</li>
        </ul>

        <h3 style="color: #00ff88;">🔌 نحوه سیم‌کشی و اتصال (Wiring)</h3>
        <p style="line-height: 1.5;">
            برای وصل کردن دو قطعه، کافی است روی <b>پین خروجی</b> قطعه اول کلیک کرده و سپس روی <b>پین ورودی</b> قطعه دوم کلیک کنید تا سیم بین آن‌ها برقرار شود.
        </p>

        <h3 style="color: #00ff88;">💡 نکات برای عملکرد بهتر</h3>
        <ul>
            <li>در صورت کندی شبیه‌سازی، فیلترشکن (VPN) خود را خاموش کنید.</li>
            <li>پیش از ساخت پروژه جدید، اندازه صفحه (A4 یا A3) را متناسب با حجم مدار انتخاب کنید.</li>
        </ul>
        <br/>
        <p style="text-align: center; color: #94a3b8; font-size: 11px;">PROMETHEUS SIMULATOR - Version 1.1</p>
    )";

    helpView->setHtml(helpContent);
    layout.addWidget(helpView);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, &dialog);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    layout.addWidget(&buttonBox, 0, Qt::AlignCenter);

    dialog.exec();
}

// ---------------------------------------------------------------------
// ۴. پنجره یکپارچه تنظیمات (Preferences Modal)
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
    // در loadRecentProjects
    for (const QString &filePath : qAsConst(recentFiles)) {
        QFileInfo fileInfo(filePath);
        if (!fileInfo.exists()) continue; // نادیده گرفتن فایل‌های حذف شده

        QListWidgetItem *item = new QListWidgetItem(recentProjectsList);
        item->setData(Qt::UserRole, filePath);
        item->setSizeHint(QSize(0, 44)); // ارتفاع مناسب برای نمایش دو خط

        // ساخت Widget اختصاصی برای هر آیتم لیست
        QWidget *itemWidget = new QWidget();
        QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);
        itemLayout->setContentsMargins(8, 4, 8, 4);
        itemLayout->setSpacing(2);

        QLabel *nameLbl = new QLabel(fileInfo.fileName(), itemWidget);
        nameLbl->setStyleSheet("color: #00f3ff; font-weight: bold; font-size: 13px;");

        // تاریخ تغییر و مسیر فایل
        QString lastModified = fileInfo.lastModified().toString("yyyy/MM/dd hh:mm");
        QLabel *pathLbl = new QLabel(lastModified + "  •  " + fileInfo.absolutePath(), itemWidget);
        pathLbl->setStyleSheet("color: #64748b; font-size: 10px;");

        itemLayout->addWidget(nameLbl);
        itemLayout->addWidget(pathLbl);

        recentProjectsList->addItem(item);
        recentProjectsList->setItemWidget(item, itemWidget);
    }
    for (const QString &filePath : qAsConst(recentFiles)) {
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