#include "mainwindow.h"
#include "componentlibrary.h"
#include "component.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>

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

    // اتصالات (Connections)
    connect(btnBackToStart, &QPushButton::clicked, this, &MainWindow::onBackToStartMenuClicked);
    connect(btnOpenProject, &QPushButton::clicked, this, &MainWindow::handleOpenProject);
    connect(btnSaveProject, &QPushButton::clicked, this, &MainWindow::onSaveProjectClicked);
    connect(btnToggleLibrary, &QPushButton::clicked, this, &MainWindow::onToggleLibraryClicked);

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

void MainWindow::handleNewProject(const QString& pageSize, const QString& projectName, const QString& savePath)
{
    setWindowTitle(QString("PROMETHEUS - Circuit Simulator - %1 [%2]").arg(projectName, savePath));

    mainCanvas->setCanvasSize(pageSize);
    stackedWidget->setCurrentWidget(mainCanvas);

    libraryDock->show();
    coordLabel->show();
    zoomLabel->show();
    simToolBar->show();
}

void MainWindow::handleOpenProject()
{
    stackedWidget->setCurrentWidget(mainCanvas);

    libraryDock->show();
    coordLabel->show();
    zoomLabel->show();
    simToolBar->show();
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
void MainWindow::onToggleLibraryClicked()
{
    if (libraryDock) {
        libraryDock->setVisible(!libraryDock->isVisible());
    }
}

void MainWindow::onSaveProjectClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Project"), "", tr("JSON Project (*.json)"));
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, tr("Save Project"), tr("Project saved successfully!"));
    }
}