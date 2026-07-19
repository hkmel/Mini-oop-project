#include "mainwindow.h"
#include "componentlibrary.h"
#include "component.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>   // اضافه شده برای ساخت نوار ابزار همیشه روشن موسیقی
#include <QComboBox>  // اضافه شده برای منوی کشویی آهنگ‌ها
#include <QSlider>    // اضافه شده برای اسلایدر ولوم
#include <QLabel>     // اضافه شده برای برچسب‌های متنی

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), activeComponent(nullptr)
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    startMenu = new StartMenu(this);
    stackedWidget->addWidget(startMenu);

    mainCanvas = new MainCanvas(this);
    stackedWidget->addWidget(mainCanvas);

    // اتصال سیگنال‌های منوی شروع
    connect(startMenu, &StartMenu::newProjectRequested, this, &MainWindow::handleNewProject);
    connect(startMenu, &StartMenu::openProjectRequested, this, &MainWindow::handleOpenProject);

    // اتصال سیگنال‌های بوم طراحی
    connect(mainCanvas, &MainCanvas::mouseMoved, this, &MainWindow::onMouseMoved);
    connect(mainCanvas, &MainCanvas::zoomChanged, this, &MainWindow::onZoomChanged);
    connect(mainCanvas, &MainCanvas::componentSelected, this, &MainWindow::onComponentSelected);

    // دسلکت کردن لیست قطعات فعال به محض درج موفقیت‌آمیز یک قطعه روی بوم
    connect(mainCanvas, &MainCanvas::componentPlaced, this, [this]() {
        if (activeList) {
            activeList->clearSelection();
        }
    });

    // 🎵 ۱. مقداردهی اولیه موتور صوتی برنامه (تنظیم ولوم اولیه روی ۷۰٪)
    bgMusic = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    bgMusic->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.70);
    bgMusic->setLoops(QMediaPlayer::Infinite);

    // 🎵 ۲. ساخت نوار ابزار اختصاصی موسیقی (همیشه از ابتدای برنامه بالای صفحه ظاهر می‌شود)
    QToolBar* musicToolBar = new QToolBar(tr("پخش‌کننده لوفای"), this);
    musicToolBar->setMovable(false); // ثابت کردن بار برای تمیزی ظاهر گرافیکی

    // استایل‌دهی تیره و شیک به تولبار صوتی برای هماهنگی با عکس آنا و تم برنامه
    musicToolBar->setStyleSheet(
        "QToolBar { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1a1a1a, stop:1 #2b2b2b); border-bottom: 1px solid #333; padding: 4px; spacing: 12px; }"
        "QLabel { color: #00ff88; font-weight: bold; font-family: 'Segoe UI'; font-size: 11px; }"
        "QComboBox { background: #333; color: white; border: 1px solid #555; border-radius: 3px; padding: 3px 10px; min-width: 120px; }"
        "QComboBox::drop-down { border: none; }"
        "QSlider::groove:horizontal { height: 4px; background: #444; border-radius: 2px; }"
        "QSlider::handle:horizontal { background: #00ff88; width: 12px; height: 12px; margin: -4px 0; border-radius: 6px; }"
        );
    addToolBar(Qt::TopToolBarArea, musicToolBar);

    // اضافه کردن المان‌های کنترل موزیک به تولبار پنجره اصلی
    QLabel* lblSong = new QLabel(tr("🎵 قطعه موسیقی:"), this);
    musicToolBar->addWidget(lblSong);

    QComboBox* musicCombo = new QComboBox(this);
    musicCombo->addItem("Voss", "voss");
    musicCombo->addItem("Dream Odyssey", "dream_odyssey");
    musicToolBar->addWidget(musicCombo);

    musicToolBar->addSeparator();

    QLabel* lblVol = new QLabel(tr("🔊 ولوم صدا:"), this);
    musicToolBar->addWidget(lblVol);

    QSlider* volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(70);
    volumeSlider->setFixedWidth(130);
    musicToolBar->addWidget(volumeSlider);

    // اتصالات سیگنال تولبار صوتی
    connect(musicCombo, &QComboBox::activated, this, [this, musicCombo](int index) {
        QString song = musicCombo->itemData(index).toString();
        onMusicSelected(song);
    });

    connect(volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        if (audioOutput) {
            audioOutput->setVolume(value / 100.0);
        }
    });

    // راه‌اندازی سایدبار و نوار وضعیت قطعات
    initWorkspaceWidgets();

    // 🌟 اعمال عکس بک‌گراند روی کل پنجره اصلی
    this->setStyleSheet(
        "MainWindow {"
        "   border-image: url(':/image/ANNA.jpg') 0 0 0 0 stretch stretch;"
        "   background-position: center;"
        "   background-attachment: fixed;"
        "}"
        );

    // پخش فوری و اتوماتیک آهنگ Voss در پس‌زمینه به محض اجرای پروژه
    onMusicSelected("voss");
}

MainWindow::~MainWindow() {}

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

    libraryDock->hide(); // این همان خطی است که کل سایدبار را اول کار مخفی می‌کرد!

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
}

void MainWindow::handleOpenProject()
{
    stackedWidget->setCurrentWidget(mainCanvas);

    libraryDock->show();
    coordLabel->show();
    zoomLabel->show();
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