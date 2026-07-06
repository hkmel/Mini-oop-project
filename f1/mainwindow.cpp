#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->setWindowTitle("Proteus OOP Simulator");
    this->resize(1200, 800);

    stackedWidget=new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    startMenu=new StartMenu(this);
    stackedWidget->addWidget(startMenu);

    mainCanvas=new MainCanvas(this);
    stackedWidget->addWidget(mainCanvas);

    connect(startMenu,&StartMenu::newProjectRequested,this, &MainWindow::handleNewProject);
    connect(startMenu,&StartMenu::openProjectRequested,this, &MainWindow::handleOpenProject);

    stackedWidget->setCurrentWidget(startMenu);
}

MainWindow::~MainWindow() {}

void MainWindow::handleNewProject(const QString &pageSize) {
    mainCanvas->setCanvasSize(pageSize);
    stackedWidget->setCurrentWidget(mainCanvas);
}

void MainWindow::handleOpenProject() {
    stackedWidget->setCurrentWidget(mainCanvas);
}