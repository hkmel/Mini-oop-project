#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "startmenu.h"
#include "maincanvas.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent=nullptr);
    ~MainWindow();

private slots:
    void handleNewProject(const QString &pageSize);
    void handleOpenProject();

private:
    QStackedWidget *stackedWidget;
    StartMenu *startMenu;
    MainCanvas *mainCanvas;
};

#endif