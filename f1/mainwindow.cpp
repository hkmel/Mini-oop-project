#include "mainwindow.h"
#include "componentlibrary.h"
#include "component.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>

// #include "componentpreviewwidget.h" // چون فایل پیدا نمی‌شد، کامنت ماند

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

    // راه‌اندازی سایدبار و نوار وضعیت
    initWorkspaceWidgets();

    // 🌟 اعمال عکس بک‌گراند روی کل پنجره اصلی (جایگزین مسیر عکس خودت کن)
    // شکل استاندارد و دقیق آدرس‌دهی ری‌سورس Qt در StyleSheet
    // استفاده از border-image برای پر کردن کامل صفحه و حذف حاشیه‌ها
    this->setStyleSheet(
        "MainWindow {"
        "   border-image: url(':/image/ANNA.jpg') 0 0 0 0 stretch stretch;"
        "   background-position: center;"
        "   background-attachment: fixed;"
        "}"
        );}

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

    // جایگزین موقت ویجت پیش‌نمایش برای جلوگیری از به هم ریختن ظاهر گرافیکی پنل
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