#include "mainwindow.h"
#include "componentlibrary.h"
#include "component.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), activeComponent(nullptr), elapsedSeconds(0)
{
    setWindowTitle(tr("PROMETHEUS - Circuit Simulator"));

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    startMenu = new StartMenu(this);
    stackedWidget->addWidget(startMenu);

    mainCanvas = new MainCanvas(this);
    stackedWidget->addWidget(mainCanvas);

    // اتصال سیگنال‌های منوی شروع
    connect(startMenu, &StartMenu::newProjectRequested, this, &MainWindow::handleNewProject);
    connect(startMenu, &StartMenu::openProjectRequested, this, &MainWindow::handleOpenProject);

    // اتصال سیگنال‌های موسیقی منوی استارت
    connect(startMenu, &StartMenu::musicChanged, this, &MainWindow::onMusicSelected);
    connect(startMenu, &StartMenu::volumeChanged, this, [this](int value) {
        if (audioOutput) {
            audioOutput->setVolume(value / 100.0);
        }
    });

    // اتصال سیگنال‌های بوم طراحی
    connect(mainCanvas, &MainCanvas::mouseMoved, this, &MainWindow::onMouseMoved);
    connect(mainCanvas, &MainCanvas::zoomChanged, this, &MainWindow::onZoomChanged);
    connect(mainCanvas, &MainCanvas::componentSelected, this, &MainWindow::onComponentSelected);

    connect(mainCanvas, &MainCanvas::componentPlaced, this, [this]() {
        if (activeList) {
            activeList->clearSelection();
        }
    });

    // 🎵 مقداردهی اولیه موتور صوتی
    bgMusic = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    bgMusic->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.70);
    bgMusic->setLoops(QMediaPlayer::Infinite);

    // راه‌اندازی ابزارها و نوار شبیه‌سازی
    initWorkspaceWidgets();
    createSimulationToolBar();

    // پخش اولیه اتوماتیک ترک Voss
    onMusicSelected("voss");
}

MainWindow::~MainWindow() {}

// 🌟 ساخت نوار کنترل شبیه‌سازی (Run / Pause / Stop / Timer)
void MainWindow::createSimulationToolBar()
{
    simToolBar = addToolBar(tr("Simulation Controls"));
    simToolBar->setMovable(false);
    simToolBar->setStyleSheet(
        "QToolBar {"
        "   background: rgba(8, 14, 28, 230);"
        "   border-bottom: 1px solid rgba(0, 243, 255, 0.4);"
        "   spacing: 12px;"
        "   padding: 6px 12px;"
        "}"
        );

    // دکمه Run
    btnRun = new QPushButton("▶ Run", this);
    btnRun->setCursor(Qt::PointingHandCursor);
    btnRun->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(0, 200, 83, 0.2);"
        "   color: #00e676;"
        "   border: 1px solid #00e676;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   padding: 5px 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #00e676;"
        "   color: #050b14;"
        "}"
        );

    // دکمه Pause
    btnPause = new QPushButton("⏸ Pause", this);
    btnPause->setCursor(Qt::PointingHandCursor);
    btnPause->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 145, 0, 0.2);"
        "   color: #ffab40;"
        "   border: 1px solid #ffab40;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   padding: 5px 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #ffab40;"
        "   color: #050b14;"
        "}"
        );

    // دکمه Stop
    btnStop = new QPushButton("⏹ Stop", this);
    btnStop->setCursor(Qt::PointingHandCursor);
    btnStop->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 23, 68, 0.2);"
        "   color: #ff5252;"
        "   border: 1px solid #ff5252;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   padding: 5px 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #ff5252;"
        "   color: #ffffff;"
        "}"
        );

    // نمایشگر تایمر با فونت دیجیتال نئونی
    timerDisplayLabel = new QLabel("⏱ 00:00:00", this);
    timerDisplayLabel->setStyleSheet(
        "QLabel {"
        "   background-color: #050a14;"
        "   color: #00f3ff;"
        "   font-family: 'Consolas', 'Courier New', monospace;"
        "   font-size: 15px;"
        "   font-weight: bold;"
        "   border: 1px solid rgba(0, 243, 255, 0.6);"
        "   border-radius: 6px;"
        "   padding: 4px 14px;"
        "}"
        );

    simToolBar->addWidget(btnRun);
    simToolBar->addWidget(btnPause);
    simToolBar->addWidget(btnStop);
    simToolBar->addSeparator();
    simToolBar->addWidget(timerDisplayLabel);

    // اتصال سیگنال‌های دکمه‌ها
    connect(btnRun, &QPushButton::clicked, this, &MainWindow::onRunSimulation);
    connect(btnPause, &QPushButton::clicked, this, &MainWindow::onPauseSimulation);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::onStopSimulation);

    // تنظیم تایمر اصلی (هر ۱۰۰۰ میلی‌ثانیه = ۱ ثانیه)
    simTimer = new QTimer(this);
    connect(simTimer, &QTimer::timeout, this, &MainWindow::onUpdateSimTimer);

    // مخفی کردن نوار شبیه‌سازی در صفحه استارت‌منو
    simToolBar->hide();
}

// 🟢 شروع / ادامه شبیه‌سازی
void MainWindow::onRunSimulation()
{
    if (!simTimer->isActive()) {
        simTimer->start(1000);
    }
}

// 🟠 توقف موقت (Pause)
void MainWindow::onPauseSimulation()
{
    if (simTimer->isActive()) {
        simTimer->stop();
    }
}

// 🔴 استاپ کامل و ریست شدن تایمر به صفر
void MainWindow::onStopSimulation()
{
    simTimer->stop();
    elapsedSeconds = 0;

    // به‌روزرسانی آنی نمایشگر
    timerDisplayLabel->setText("⏱ 00:00:00");
}

// ⏱ آپدیت متغیر زمان و فرمت نمایش (HH:MM:SS)
void MainWindow::onUpdateSimTimer()
{
    elapsedSeconds++;

    int hrs = elapsedSeconds / 3600;
    int mins = (elapsedSeconds % 3600) / 60;
    int secs = elapsedSeconds % 60;

    QString timeStr = QString("⏱ %1:%2:%3")
                          .arg(hrs, 2, 10, QChar('0'))
                          .arg(mins, 2, 10, QChar('0'))
                          .arg(secs, 2, 10, QChar('0'));

    timerDisplayLabel->setText(timeStr);
}

void MainWindow::initWorkspaceWidgets()
{
    libraryDock = new QDockWidget(tr("کتابخانه قطعات"), this);
    libraryDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QWidget* dockContents = new QWidget(libraryDock);
    QVBoxLayout* layout = new QVBoxLayout(dockContents);

    searchEdit = new QLineEdit(dockContents);
    searchEdit->setPlaceholderText(tr("جستجوی قطعه یا دسته بندی..."));
    layout->addWidget(searchEdit);
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::filterLibrary);

    libraryTree = new QTreeWidget(dockContents);
    libraryTree->setHeaderLabel(tr("دسته بندی المان ها"));
    layout->addWidget(libraryTree);
    connect(libraryTree, &QTreeWidget::itemClicked, this, &MainWindow::onTreeItemClicked);

    previewWidgetPlaceholder = new QWidget(dockContents);
    previewWidgetPlaceholder->setMinimumSize(150, 20);
    layout->addWidget(previewWidgetPlaceholder);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnAddActive = new QPushButton(tr("<< اضافه به فعال"), dockContents);
    btnRemoveActive = new QPushButton(tr("حذف از فعال >>"), dockContents);
    btnLayout->addWidget(btnAddActive);
    btnLayout->addWidget(btnRemoveActive);
    layout->addLayout(btnLayout);

    connect(btnAddActive, &QPushButton::clicked, this, &MainWindow::onAddActiveClicked);
    connect(btnRemoveActive, &QPushButton::clicked, this, &MainWindow::onRemoveActiveClicked);

    activeList = new QListWidget(dockContents);
    layout->addWidget(new QLabel(tr("قطعات فعال پروژه (Devices):"), dockContents));
    layout->addWidget(activeList);
    connect(activeList, &QListWidget::itemClicked, this, &MainWindow::onActiveListClicked);

    dockContents->setLayout(layout);
    libraryDock->setWidget(dockContents);
    addDockWidget(Qt::LeftDockWidgetArea, libraryDock);

    libraryDock->hide();

    coordLabel = new QLabel(tr("مختصات: (0, 0)"), this);
    zoomLabel = new QLabel(tr("بزرگ نمایی: 100%"), this);

    coordLabel->hide();
    zoomLabel->hide();

    statusBar()->addPermanentWidget(coordLabel);
    statusBar()->addPermanentWidget(zoomLabel);

    filterLibrary("");
}

void MainWindow::handleNewProject(const QString& pageSize)
{
    mainCanvas->setCanvasSize(pageSize);
    stackedWidget->setCurrentWidget(mainCanvas);

    libraryDock->show();
    coordLabel->show();
    zoomLabel->show();
    simToolBar->show(); // 🌟 نمایش نوار ابزار شبیه‌سازی هنگام ورود به بوم
}

void MainWindow::handleOpenProject()
{
    stackedWidget->setCurrentWidget(mainCanvas);

    libraryDock->show();
    coordLabel->show();
    zoomLabel->show();
    simToolBar->show(); // 🌟 نمایش نوار ابزار شبیه‌سازی هنگام ورود به بوم
}

void MainWindow::filterLibrary(const QString& text)
{
    libraryTree->clear();
    ComponentLibrary& lib = ComponentLibrary::getInstance();
    QStringList categories = lib.getCategories();
    int totalFound = 0;

    for (const QString& cat : categories) {
        QStringList components = lib.getComponentsInCategory(cat);
        QStringList filteredComponents;

        bool catMatches = cat.contains(text, Qt::CaseInsensitive);

        for (const QString& comp : components) {
            if (catMatches || comp.contains(text, Qt::CaseInsensitive)) {
                filteredComponents.append(comp);
            }
        }

        if (!filteredComponents.isEmpty()) {
            QTreeWidgetItem* catItem = new QTreeWidgetItem(libraryTree);
            catItem->setText(0, cat);
            catItem->setExpanded(true);

            for (const QString& comp : filteredComponents) {
                QTreeWidgetItem* compItem = new QTreeWidgetItem(catItem);
                compItem->setText(0, comp);
            }
            totalFound += filteredComponents.size();
        }
    }

    if (totalFound == 0 && !text.isEmpty()) {
        QTreeWidgetItem* noResultItem = new QTreeWidgetItem(libraryTree);
        noResultItem->setText(0, tr("قطعه ای پیدا نشد!"));
    }
}

void MainWindow::onTreeItemClicked(QTreeWidgetItem* item, int col)
{
    Q_UNUSED(col);
    if (!item || item->parent() == nullptr) {
        return;
    }
}

void MainWindow::onAddActiveClicked()
{
    QTreeWidgetItem* currentItem = libraryTree->currentItem();
    if (!currentItem || currentItem->parent() == nullptr) {
        QMessageBox::warning(this, tr("خطا"), tr("لطفاً یک قطعه را از ساختار درختی انتخاب کنید."));
        return;
    }

    QString compName = currentItem->text(0);

    for (int i = 0; i < activeList->count(); ++i) {
        if (activeList->item(i)->text() == compName) {
            return;
        }
    }
    activeList->addItem(compName);
}

void MainWindow::onRemoveActiveClicked()
{
    QListWidgetItem* item = activeList->currentItem();
    if (item) {
        delete activeList->takeItem(activeList->row(item));
        mainCanvas->setCurrentSelectedType("");
    }
}

void MainWindow::onActiveListClicked(QListWidgetItem* item)
{
    if (!item) return;
    mainCanvas->setCurrentSelectedType(item->text());
}

void MainWindow::onMouseMoved(const QPointF& pos)
{
    coordLabel->setText(QString("مختصات: (%1, %2)").arg(int(pos.x())).arg(int(pos.y())));
}

void MainWindow::onZoomChanged(int percentage)
{
    zoomLabel->setText(QString("بزرگ نمایی: %1%").arg(percentage));
}

void MainWindow::onComponentSelected(Component* comp)
{
    activeComponent = comp;
}

void MainWindow::onPropertyValueChanged(QTableWidgetItem* item)
{
    Q_UNUSED(item);
}

void MainWindow::onMusicSelected(const QString& songName) {
    if (!bgMusic) return;

    bgMusic->stop();

    QString filePath;
    if (songName == "voss") {
        filePath = "qrc:/music/voss.mp3";
    } else if (songName == "dream_odyssey") {
        filePath = "qrc:/music/dream_odyssey.mp3";
    }

    bgMusic->setSource(QUrl(filePath));
    bgMusic->play();
}