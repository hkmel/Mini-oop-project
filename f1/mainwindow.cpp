
#include "mainwindow.h"
#include "componentlibrary.h"
#include "component.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QSequentialAnimationGroup>
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

    btnUndo = new QPushButton("↩️ Undo", this);
    btnUndo->setCursor(Qt::PointingHandCursor);
    btnUndo->setStyleSheet(btnBackToStart->styleSheet());
    btnUndo->setToolTip("Undo (Ctrl+Z)");

    btnRedo = new QPushButton("↪️ Redo", this);
    btnRedo->setCursor(Qt::PointingHandCursor);
    btnRedo->setStyleSheet(btnBackToStart->styleSheet());
    btnRedo->setToolTip("Redo (Ctrl+Y)");

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

    // ساخت دکمه قلب
    btnFavorite = new QPushButton("🤍", this);
    btnFavorite->setCursor(Qt::PointingHandCursor);
    btnFavorite->setToolTip("افزودن به مدارهای مورد علاقه");
    btnFavorite->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 0, 100, 0.1);"
        "   color: #ff007f;"
        "   border: 1px solid rgba(255, 0, 100, 0.4);"
        "   border-radius: 6px;"
        "   font-size: 14px;"
        "   padding: 5px 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 0, 127, 0.3);"
        "   border-color: #ff007f;"
        "}"
        );

    // ساخت اکشن ذخیره تصویر
    QAction* exportImageAction = new QAction("Save as Image", this);
    exportImageAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    exportImageAction->setToolTip("ذخیره مدار به صورت تصویر PNG یا JPG");

    // افزودن المان‌ها به ترتیب صحیح به نوار ابزار
    simToolBar->addWidget(btnBackToStart);
    simToolBar->addSeparator();
    simToolBar->addWidget(btnOpenProject);
    simToolBar->addWidget(btnSaveProject);
    simToolBar->addWidget(btnUndo);
    simToolBar->addWidget(btnRedo);
    simToolBar->addSeparator();
    simToolBar->addWidget(btnToggleLibrary);
    simToolBar->addSeparator();
    simToolBar->addWidget(btnZoomIn);
    simToolBar->addWidget(btnZoomOut);
    simToolBar->addWidget(btnZoomFit);
    simToolBar->addSeparator();

    // اضافه کردن Save as Image و بلافاصله دکمه قلب در سمت راست آن
    simToolBar->addAction(exportImageAction);
    simToolBar->addWidget(btnFavorite);

    // اتصالات (Connections)
    connect(btnBackToStart, &QPushButton::clicked, this, &MainWindow::onBackToStartMenuClicked);
    connect(btnOpenProject, &QPushButton::clicked, this, &MainWindow::handleOpenProject);
    connect(btnSaveProject, &QPushButton::clicked, this, &MainWindow::onSaveProjectClicked);
    connect(btnUndo, &QPushButton::clicked, mainCanvas, &MainCanvas::undo);
    connect(btnRedo, &QPushButton::clicked, mainCanvas, &MainCanvas::redo);
    connect(btnToggleLibrary, &QPushButton::clicked, this, &MainWindow::onToggleLibraryClicked);

    connect(btnZoomIn, &QPushButton::clicked, mainCanvas, &MainCanvas::zoomIn);
    connect(btnZoomOut, &QPushButton::clicked, mainCanvas, &MainCanvas::zoomOut);
    connect(btnZoomFit, &QPushButton::clicked, mainCanvas, &MainCanvas::zoomToFit);

    connect(exportImageAction, &QAction::triggered, mainCanvas, &MainCanvas::exportToImage);
    connect(btnFavorite, &QPushButton::clicked, this, &MainWindow::onFavoriteClicked);

    // اعمال استایل اختصاصی مود پرومتیوس
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
    // ۱. ساخت و تنظیمات کامل Dock Widget کتابخانه قطعات با تم پرومتیوس
    // ---------------------------------------------------------------------
    libraryDock = new QDockWidget(tr("Component Library"), this);
    libraryDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // استایل‌دهی قاب اصلی Dock Widget (عنوان و کادر دور)
    libraryDock->setStyleSheet(
        "QDockWidget {"
        "    color: #00f3ff;"
        "    font-weight: bold;"
        "    font-size: 13px;"
        "    titlebar-close-icon: url(close.png);"
        "    titlebar-normal-icon: url(undock.png);"
        "}"
        "QDockWidget::title {"
        "    background-color: #080e1c;"
        "    text-align: left;"
        "    padding-left: 8px;"
        "    padding-top: 4px;"
        "    padding-bottom: 4px;"
        "    border-bottom: 1px solid #00f3ff;"
        "}"
        );

    QWidget* dockContents = new QWidget(libraryDock);
    dockContents->setStyleSheet("background-color: #050b14;"); // پس‌زمینه تیره و یکدست پنل
    QVBoxLayout* layout = new QVBoxLayout(dockContents);

    // ۱-۱. باکس جستجو (QLineEdit)
    searchEdit = new QLineEdit(dockContents);
    searchEdit->setPlaceholderText(tr("Search component..."));
    searchEdit->setStyleSheet(
        "QLineEdit {"
        "    background-color: #0d1527;"
        "    color: #e2e8f0;"
        "    border: 1px solid rgba(0, 243, 255, 0.4);"
        "    border-radius: 5px;"
        "    padding: 6px 10px;"
        "    font-size: 12px;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #00f3ff;"
        "    background-color: #111a2e;"
        "}"
        );
    layout->addWidget(searchEdit);
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::filterLibrary);

    // ۱-۲. درخت قطعات (QTreeWidget)
    libraryTree = new QTreeWidget(dockContents);
    libraryTree->setHeaderLabel(tr("Categories"));
    libraryTree->setStyleSheet(
        "QTreeWidget {"
        "    background-color: #080e1c;"
        "    color: #cbd5e1;"
        "    border: 1px solid #1e293b;"
        "    border-radius: 5px;"
        "    font-size: 12px;"
        "}"
        "QTreeWidget::header {"
        "    background-color: #0d1a30;"
        "    color: #00f3ff;"
        "    border-bottom: 1px solid #00f3ff;"
        "    font-weight: bold;"
        "    padding: 4px;"
        "}"
        "QTreeWidget::item {"
        "    padding: 4px;"
        "}"
        "QTreeWidget::item:hover {"
        "    background-color: rgba(0, 243, 255, 0.15);"
        "    color: #00f3ff;"
        "}"
        "QTreeWidget::item:selected {"
        "    background-color: #00f3ff;"
        "    color: #050b14;"
        "    font-weight: bold;"
        "}"
        );
    layout->addWidget(libraryTree);
    connect(libraryTree, &QTreeWidget::itemClicked, this, &MainWindow::onTreeItemClicked);

    // ۱-۳. نگهدارنده پیش‌نمایش
    previewWidgetPlaceholder = new QWidget(dockContents);
    previewWidgetPlaceholder->setMinimumSize(150, 20);
    layout->addWidget(previewWidgetPlaceholder);

    // ۱-۴. دکمه‌های افزودن و حذف (QPushButton)
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnAddActive = new QPushButton(tr("<< Add to Active"), dockContents);
    btnRemoveActive = new QPushButton(tr("Remove >>"), dockContents);

    QString buttonStyle =
        "QPushButton {"
        "    background-color: rgba(0, 243, 255, 0.1);"
        "    color: #00f3ff;"
        "    border: 1px solid rgba(0, 243, 255, 0.4);"
        "    border-radius: 5px;"
        "    padding: 6px 10px;"
        "    font-weight: bold;"
        "    font-size: 11px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #00f3ff;"
        "    color: #050b14;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #00b8c4;"
        "    color: #000000;"
        "}";

    btnAddActive->setStyleSheet(buttonStyle);
    btnRemoveActive->setStyleSheet(buttonStyle);
    btnAddActive->setCursor(Qt::PointingHandCursor);
    btnRemoveActive->setCursor(Qt::PointingHandCursor);

    btnLayout->addWidget(btnAddActive);
    btnLayout->addWidget(btnRemoveActive);
    layout->addLayout(btnLayout);

    connect(btnAddActive, &QPushButton::clicked, this, &MainWindow::onAddActiveClicked);
    connect(btnRemoveActive, &QPushButton::clicked, this, &MainWindow::onRemoveActiveClicked);

    // ۱-۵. لیست قطعات فعال (QListWidget)
    QLabel* activeListLabel = new QLabel(tr("Active Devices:"), dockContents);
    activeListLabel->setStyleSheet("color: #38bdf8; font-weight: bold; font-size: 12px; margin-top: 4px;");
    layout->addWidget(activeListLabel);

    activeList = new QListWidget(dockContents);
    activeList->setStyleSheet(
        "QListWidget {"
        "    background-color: #080e1c;"
        "    color: #e2e8f0;"
        "    border: 1px solid #1e293b;"
        "    border-radius: 5px;"
        "    font-size: 12px;"
        "}"
        "QListWidget::item {"
        "    padding: 5px;"
        "    border-bottom: 1px solid #0d1527;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: rgba(0, 243, 255, 0.1);"
        "    color: #00f3ff;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #00f3ff;"
        "    color: #050b14;"
        "    font-weight: bold;"
        "}"
        );
    layout->addWidget(activeList);
    connect(activeList, &QListWidget::itemClicked, this, &MainWindow::onActiveListClicked);

    dockContents->setLayout(layout);
    libraryDock->setWidget(dockContents);
    addDockWidget(Qt::LeftDockWidgetArea, libraryDock);

    libraryDock->hide();

    // ---------------------------------------------------------------------
    // ۲. استایل‌دهی نوار وضعیت (QStatusBar)
    // ---------------------------------------------------------------------
    statusBar()->setStyleSheet(
        "QStatusBar {"
        "    background-color: #0b0e14;"
        "    border-top: 1px solid #00aa00;"
        "}"
        "QStatusBar::item {"
        "    border: none;"
        "}"
        );

    QString statusLabelStyle =
        "QLabel {"
        "    color: #39FF14;"
        "    background-color: #041204;"
        "    border: 1px solid #00aa00;"
        "    border-radius: 4px;"
        "    padding: 3px 10px;"
        "    font-family: 'Consolas', 'Monaco', 'Courier New', monospace;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "}";

    coordLabel = new QLabel("X: +0   Y: +0", this);
    coordLabel->setStyleSheet(statusLabelStyle);

    zoomLabel = new QLabel("Zoom: 100%", this);
    zoomLabel->setStyleSheet(statusLabelStyle);

    coordLabel->show();
    zoomLabel->show();

    statusBar()->addPermanentWidget(coordLabel);
    statusBar()->addPermanentWidget(zoomLabel);

    // ---------------------------------------------------------------------
    // ۳. بارگذاری اولیه لیست کتابخانه
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

    QShortcut *undoShortcut = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(undoShortcut, &QShortcut::activated, mainCanvas, &MainCanvas::undo);

    QShortcut *redoShortcut1 = new QShortcut(QKeySequence("Ctrl+Y"), this);
    connect(redoShortcut1, &QShortcut::activated, mainCanvas, &MainCanvas::redo);

    QShortcut *redoShortcut2 = new QShortcut(QKeySequence("Ctrl+Shift+Z"), this);
    connect(redoShortcut2, &QShortcut::activated, mainCanvas, &MainCanvas::redo);

    QShortcut *zoomInReq = new QShortcut(QKeySequence("Ctrl+I"), this);
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
void MainWindow::onFavoriteClicked()
{
    // ۱. انیمیشن تپش (Heartbeat Effect) برای دکمه
    QGraphicsOpacityEffect *btnEffect = new QGraphicsOpacityEffect(btnFavorite);
    btnFavorite->setGraphicsEffect(btnEffect);

    QSequentialAnimationGroup *heartPulse = new QSequentialAnimationGroup(btnFavorite);

    QPropertyAnimation *anim1 = new QPropertyAnimation(btnEffect, "opacity");
    anim1->setDuration(120);
    anim1->setStartValue(1.0);
    anim1->setEndValue(0.3);

    QPropertyAnimation *anim2 = new QPropertyAnimation(btnEffect, "opacity");
    anim2->setDuration(120);
    anim2->setStartValue(0.3);
    anim2->setEndValue(1.0);

    heartPulse->addAnimation(anim1);
    heartPulse->addAnimation(anim2);
    heartPulse->start(QAbstractAnimation::DeleteWhenStopped);

    // ۲. تغییر حالت دکمه و نمایش پیام Toast
    if (btnFavorite->text() == "🤍") {
        btnFavorite->setText("❤️");
        btnFavorite->setStyleSheet(
            "QPushButton {"
            "   background-color: #ff0055;"
            "   color: #ffffff;"
            "   border: 1px solid #ff0055;"
            "   border-radius: 6px;"
            "   font-size: 14px;"
            "   padding: 5px 12px;"
            "}"
            );
        showToastNotification("✨ این مدار به لیست مدارهای مورد علاقه‌ات اضافه شد!");
    } else {
        btnFavorite->setText("🤍");
        btnFavorite->setStyleSheet(
            "QPushButton {"
            "   background-color: rgba(255, 0, 100, 0.1);"
            "   color: #ff007f;"
            "   border: 1px solid rgba(255, 0, 100, 0.4);"
            "   border-radius: 6px;"
            "   font-size: 14px;"
            "   padding: 5px 12px;"
            "}"
            );
        showToastNotification("💔 مدار از لیست مورد علاقه‌ها حذف شد.");
    }
}

void MainWindow::showToastNotification(const QString &message)
{
    // ساخت بنر اعلان با استایل نئون مدرن
    QLabel *toast = new QLabel(message, this);
    toast->setStyleSheet(
        "QLabel {"
        "   background-color: rgba(8, 14, 28, 230);"
        "   color: #00f3ff;"
        "   border: 1px solid #00f3ff;"
        "   border-radius: 10px;"
        "   padding: 8px 18px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   font-family: 'Segoe UI', sans-serif;"
        "}"
        );

    // افزودن افکت درخشش نئون
    QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(toast);
    glow->setBlurRadius(20);
    glow->setColor(QColor(0, 243, 255, 160));
    glow->setOffset(0, 0);
    toast->setGraphicsEffect(glow);

    toast->adjustSize();

    // قرارگیری پیام در بالا و وسط کانواس
    int x = (width() - toast->width()) / 2;
    int y = 75;
    toast->move(x, y);
    toast->show();

    // انیمیشن محو شدن نرم (Fade Out)
    QGraphicsOpacityEffect *fadeEffect = new QGraphicsOpacityEffect(toast);
    toast->setGraphicsEffect(fadeEffect);

    QPropertyAnimation *fadeAnim = new QPropertyAnimation(fadeEffect, "opacity");
    fadeAnim->setDuration(2200);
    fadeAnim->setStartValue(1.0);
    fadeAnim->setKeyValueAt(0.7, 1.0); // ۷۰٪ زمان کاملاً واضح می‌ماند
    fadeAnim->setEndValue(0.0);

    // پاکسازی خودکار حافظه پس از اتمام انیمیشن
    connect(fadeAnim, &QPropertyAnimation::finished, toast, &QLabel::deleteLater);
    fadeAnim->start(QAbstractAnimation::DeleteWhenStopped);
}
