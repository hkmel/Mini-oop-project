#include "mainwindow.h"
#include "componentlibrary.h"
#include "component.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QShortcut>
#include <QKeySequence>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), activeComponent(nullptr)
{
    setWindowTitle(tr("PROMETHEUS - Circuit Simulator"));

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    startMenu = new StartMenu(this);
    stackedWidget->addWidget(startMenu);

    mainCanvas = new MainCanvas(this);
    stackedWidget->addWidget(mainCanvas);
    connect(startMenu, qOverload<const QString&, const QString&, const QString&>(&StartMenu::newProjectRequested),
            this, &MainWindow::handleNewProject);
    connect(startMenu, &StartMenu::openProjectRequested, this, &MainWindow::handleOpenProject);
    connect(startMenu, &StartMenu::musicChanged, this, &MainWindow::onMusicSelected);
    connect(startMenu, &StartMenu::volumeChanged, this, [this](int value) {
        if (audioOutput) {
            audioOutput->setVolume(value / 100.0);
        }
    });

    connect(mainCanvas, &MainCanvas::mouseMoved, this, &MainWindow::onMouseMoved);
    connect(mainCanvas, &MainCanvas::zoomChanged, this, &MainWindow::onZoomChanged);
    connect(mainCanvas, &MainCanvas::componentSelected, this, &MainWindow::onComponentSelected);

    connect(mainCanvas, &MainCanvas::componentPlaced, this, [this]() {
        if (activeList) {
            activeList->clearSelection();
        }
    });

    bgMusic = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    bgMusic->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.70);
    bgMusic->setLoops(QMediaPlayer::Infinite);

    initWorkspaceWidgets();
    createSimulationToolBar();

    onMusicSelected("voss");
    setupShortcuts();
}

MainWindow::~MainWindow() {}

void MainWindow::createSimulationToolBar()
{

    simToolBar = addToolBar(tr("Simulation & Tools Bar"));
    simToolBar->setMovable(false);
    simToolBar->setStyleSheet(
        "QToolBar {"
        "   background: rgba(8, 14, 28, 240);"
        "   border-bottom: 1px solid rgba(0, 243, 255, 0.4);"
        "   spacing: 8px;"
        "   padding: 6px 12px;"
        "}"
        );

    // ۱. مدیریت پروژه و ناوبری
    btnBackToStart = new QPushButton("🏠 Start Menu", this);
    btnBackToStart->setCursor(Qt::PointingHandCursor);
    btnBackToStart->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(0, 243, 255, 0.1);"
        "   color: #00f3ff;"
        "   border: 1px solid rgba(0, 243, 255, 0.5);"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   padding: 5px 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #00f3ff;"
        "   color: #050b14;"
        "}"
        );

    btnOpenProject = new QPushButton("📂 Open", this);
    btnOpenProject->setCursor(Qt::PointingHandCursor);
    btnOpenProject->setStyleSheet(btnBackToStart->styleSheet());

    btnSaveProject = new QPushButton("💾 Save", this);
    btnSaveProject->setCursor(Qt::PointingHandCursor);
    btnSaveProject->setStyleSheet(btnBackToStart->styleSheet());

    // ۲. کنترل نمایش پنل کتابخانه
    btnToggleLibrary = new QPushButton("📚 Library Panel", this);
    btnToggleLibrary->setCursor(Qt::PointingHandCursor);
    btnToggleLibrary->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 0.08);"
        "   color: #e2e8f0;"
        "   border: 1px solid rgba(255, 255, 255, 0.2);"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   padding: 5px 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(0, 243, 255, 0.2);"
        "   color: #00f3ff;"
        "   border-color: #00f3ff;"
        "}"
        );

    // افزودن المان‌های باقی‌مانده به نوار ابزار
    simToolBar->addWidget(btnBackToStart);
    simToolBar->addSeparator();
    simToolBar->addWidget(btnOpenProject);
    simToolBar->addWidget(btnSaveProject);
    simToolBar->addSeparator();
    simToolBar->addWidget(btnToggleLibrary);
    simToolBar->addSeparator();

    btnZoomIn = new QPushButton("🔍+", this);
    btnZoomIn->setCursor(Qt::PointingHandCursor);
    btnZoomIn->setToolTip("Zoom In");
    btnZoomIn->setStyleSheet(btnToggleLibrary->styleSheet());

    btnZoomOut = new QPushButton("🔍-", this);
    btnZoomOut->setCursor(Qt::PointingHandCursor);
    btnZoomOut->setToolTip("Zoom Out");
    btnZoomOut->setStyleSheet(btnToggleLibrary->styleSheet());

    btnZoomFit = new QPushButton("🔲 Fit", this);
    btnZoomFit->setCursor(Qt::PointingHandCursor);
    btnZoomFit->setToolTip("Zoom to Fit");
    btnZoomFit->setStyleSheet(btnToggleLibrary->styleSheet());

    simToolBar->addWidget(btnZoomIn);
    simToolBar->addWidget(btnZoomOut);
    simToolBar->addWidget(btnZoomFit);

    // اتصالات (Connections)
    connect(btnBackToStart, &QPushButton::clicked, this, &MainWindow::onBackToStartMenuClicked);
    connect(btnOpenProject, &QPushButton::clicked, this, &MainWindow::handleOpenProject);
    connect(btnSaveProject, &QPushButton::clicked, this, &MainWindow::onSaveProjectClicked);
    connect(btnToggleLibrary, &QPushButton::clicked, this, &MainWindow::onToggleLibraryClicked);
    // ساخت اکشن ذخیره تصویر
    // ۱. ساخت اکشن ذخیره تصویر
    QAction* exportImageAction = new QAction("Save as Image", this);
    exportImageAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    exportImageAction->setToolTip("ذخیره مدار به صورت تصویر PNG یا JPG");

    // ۲. اتصال signal/slot
    connect(exportImageAction, &QAction::triggered, mainCanvas, &MainCanvas::exportToImage);

    // ۳. اضافه کردن به نوار ابزار
    simToolBar->addAction(exportImageAction);

    // ۴. اعمال استایل اختصاصی مود پرومتیوس (آبی نئون + زمینه تیره + فونت روشن)
    simToolBar->setStyleSheet(
        "QToolBar {"
        "   background-color: #080e1c;"
        "   border: none;"
        "   spacing: 6px;"
        "}"
        "QToolButton {"
        "   background-color: rgba(0, 243, 255, 0.1);"
        "   color: #00f3ff;"
        "   border: 1px solid rgba(0, 243, 255, 0.4);"
        "   border-radius: 6px;"
        "   padding: 5px 12px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QToolButton:hover {"
        "   background-color: #00f3ff;"
        "   color: #050b14;"
        "   border: 1px solid #00f3ff;"
        "}"
        "QToolButton:pressed {"
        "   background-color: #00b8c4;"
        "   color: #000000;"
        "}"
        "QToolButton:disabled {"
        "   color: #475569;"
        "   border-color: #1e293b;"
        "   background-color: rgba(255, 255, 255, 0.02);"
        "}"
        );

    simToolBar->hide();
}

void MainWindow::onBackToStartMenuClicked()
{
    // فراخوانی onStopSimulation() حذف شد
    stackedWidget->setCurrentWidget(startMenu);

    libraryDock->hide();
    coordLabel->hide();
    zoomLabel->hide();
    simToolBar->hide();
}
void MainWindow::initWorkspaceWidgets()
{
    // ---------------------------------------------------------------------
    // ۱. ساخت و تنظیمات کامل Dock Widget کتابخانه قطعات (بدون تغییر)
    // ---------------------------------------------------------------------
    libraryDock = new QDockWidget(tr("component liberary"), this);
    libraryDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QWidget* dockContents = new QWidget(libraryDock);
    QVBoxLayout* layout = new QVBoxLayout(dockContents);

    searchEdit = new QLineEdit(dockContents);
    searchEdit->setPlaceholderText(tr("Search the component..."));
    layout->addWidget(searchEdit);
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::filterLibrary);

    libraryTree = new QTreeWidget(dockContents);
    libraryTree->setHeaderLabel(tr("Sort the component"));
    layout->addWidget(libraryTree);
    connect(libraryTree, &QTreeWidget::itemClicked, this, &MainWindow::onTreeItemClicked);

    previewWidgetPlaceholder = new QWidget(dockContents);
    previewWidgetPlaceholder->setMinimumSize(150, 20);
    layout->addWidget(previewWidgetPlaceholder);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnAddActive = new QPushButton(tr("<<Add to Activ"), dockContents);
    btnRemoveActive = new QPushButton(tr("Delete  >>"), dockContents);
    btnLayout->addWidget(btnAddActive);
    btnLayout->addWidget(btnRemoveActive);
    layout->addLayout(btnLayout);

    connect(btnAddActive, &QPushButton::clicked, this, &MainWindow::onAddActiveClicked);
    connect(btnRemoveActive, &QPushButton::clicked, this, &MainWindow::onRemoveActiveClicked);

    activeList = new QListWidget(dockContents);
    layout->addWidget(new QLabel(tr(" (Devices):"), dockContents));
    layout->addWidget(activeList);
    connect(activeList, &QListWidget::itemClicked, this, &MainWindow::onActiveListClicked);

    dockContents->setLayout(layout);
    libraryDock->setWidget(dockContents);
    addDockWidget(Qt::LeftDockWidgetArea, libraryDock);

    libraryDock->hide();

    // ---------------------------------------------------------------------
    // ۲. تغییرات جدید: استایل‌دهی نوار وضعیت (QStatusBar) به سبک پروتئوس
    // ---------------------------------------------------------------------

    // استایل تیره زمینه QStatusBar
    statusBar()->setStyleSheet(
        "QStatusBar {"
        "    background-color: #0b0e14;"
        "    border-top: 1px solid #003300;"
        "}"
        "QStatusBar::item {"
        "    border: none;"
        "}"
        );

    // استایل سبز فسفری نئون برای باکس‌های مختصات و زوم
    QString statusLabelStyle =
        "QLabel {"
        "    color: #39FF14;"                     /* سبز فسفری نئون */
        "    background-color: #041204;"          /* پس‌زمینه تیره */
        "    border: 1px solid #00aa00;"          /* کادر دور سبز */
        "    border-radius: 4px;"
        "    padding: 3px 10px;"
        "    font-family: 'Consolas', 'Monaco', 'Courier New', monospace;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "}";

    // ساخت لیبل مختصات با فرمت جدید پروتئوس
    coordLabel = new QLabel("X: +0   Y: +0", this);
    coordLabel->setStyleSheet(statusLabelStyle);

    // ساخت لیبل زوم با فرمت پروتئوس
    zoomLabel = new QLabel("Zoom: 100%", this);
    zoomLabel->setStyleSheet(statusLabelStyle);

    // نمایش لیبل‌ها
    coordLabel->show();
    zoomLabel->show();

    // افزودن به سمت راست نوار وضعیت
    statusBar()->addPermanentWidget(coordLabel);
    statusBar()->addPermanentWidget(zoomLabel);

    // ---------------------------------------------------------------------
    // ۳. بارگذاری اولیه لیست کتابخانه (بدون تغییر)
    // ---------------------------------------------------------------------
    filterLibrary("");
}
void MainWindow::handleNewProject(const QString& pageSize, const QString& projectName, const QString& savePath)
{
    setWindowTitle(QString("PROMETHEUS - Circuit Simulator - %1 [%2]").arg(projectName, savePath));

    // پاکسازی بوم از قطعات و سیم‌های احتمالی قبلی
    mainCanvas->clearCanvas();

    mainCanvas->setCanvasSize(pageSize);
    stackedWidget->setCurrentWidget(mainCanvas);

    libraryDock->show();
    coordLabel->show();
    zoomLabel->show();
    simToolBar->show();
}
void MainWindow::handleOpenProject()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Project"),
                                                    "",
                                                    tr("JSON Project (*.json)"));

    // اگر کاربر پنجره را بست و فایلی انتخاب نکرد
    if (fileName.isEmpty()) {
        return;
    }

    // فراخوانی تابع لود پروژه در بوم
    if (mainCanvas->loadFromFile(fileName)) {
        // تغییر عنوان پنجره به نام فایل لود شده
        QFileInfo fileInfo(fileName);
        setWindowTitle(QString("PROMETHEUS - Circuit Simulator - %1").arg(fileInfo.fileName()));

        // نمایش بوم و ابزارها
        stackedWidget->setCurrentWidget(mainCanvas);
        libraryDock->show();
        coordLabel->show();
        zoomLabel->show();
        simToolBar->show();

        QMessageBox::information(this, tr("Success"), tr("Project loaded successfully!"));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to open or parse project file!"));
    }
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
    int x = qRound(pos.x());
    int y = -qRound(pos.y());

    // فرمت‌دهی علامت + و - مشابه پروتئوس
    QString xStr = QString("%1%2").arg(x >= 0 ? "+" : "").arg(x);
    QString yStr = QString("%1%2").arg(y >= 0 ? "+" : "").arg(y);

    coordLabel->setText(QString("X: %1   Y: %2").arg(xStr, yStr));
}

void MainWindow::onZoomChanged(int percentage)
{
    zoomLabel->setText(QString("Zoom: %1%").arg(percentage));
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
void MainWindow::onToggleLibraryClicked()
{
    if (libraryDock) {
        libraryDock->setVisible(!libraryDock->isVisible());
    }
}

void MainWindow::onSaveProjectClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Project"),
                                                    "",
                                                    tr("JSON Project (*.json)"));
    if (!fileName.isEmpty()) {
        // اگر پسوند .json نداشت، به آن اضافه می‌کنیم
        if (!fileName.endsWith(".json", Qt::CaseInsensitive)) {
            fileName += ".json";
        }

        if (mainCanvas->saveToFile(fileName)) {
            QMessageBox::information(this, tr("Save Project"), tr("Project saved successfully!"));
        } else {
            QMessageBox::critical(this, tr("Save Project"), tr("Failed to save project!"));
        }
    }
}
void MainWindow::setupShortcuts()
{
    QShortcut *wireModeShortcut = new QShortcut(QKeySequence("Ctrl+W"), this);
    connect(wireModeShortcut, &QShortcut::activated, mainCanvas, &MainCanvas::toggleWireMode);

    QShortcut *zoomInReq = new QShortcut(QKeySequence("Ctrl+Z"), this);
    QShortcut *zoomInStd = new QShortcut(QKeySequence("Ctrl++"), this);
    QShortcut *zoomInStd2 = new QShortcut(QKeySequence("Ctrl+="), this);
    connect(zoomInReq, &QShortcut::activated, mainCanvas, &MainCanvas::zoomIn);
    connect(zoomInStd, &QShortcut::activated, mainCanvas, &MainCanvas::zoomIn);
    connect(zoomInStd2, &QShortcut::activated, mainCanvas, &MainCanvas::zoomIn);

    QShortcut *zoomOutStd = new QShortcut(QKeySequence("Ctrl+-"), this);
    connect(zoomOutStd, &QShortcut::activated, mainCanvas, &MainCanvas::zoomOut);

    QShortcut *zoomFitStd = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(zoomFitStd, &QShortcut::activated, mainCanvas, &MainCanvas::zoomToFit);
}


QShortcut *zoomFitReq = new QShortcut(QKeySequence("Ctrl+R"), this);
connect(zoomFitReq, &QShortcut::activated, mainCanvas, &MainCanvas::zoomToFit);