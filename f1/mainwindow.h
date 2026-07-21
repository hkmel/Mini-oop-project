#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTimer>
// 🌟 برای کنترل زمان
#include <QToolBar>       // 🌟 برای نوار ابزار شبیه‌سازی

#include "startmenu.h"
#include "maincanvas.h"

class Component;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleNewProject(const QString& pageSize);
    void handleOpenProject();
    void filterLibrary(const QString& text);
    void onTreeItemClicked(QTreeWidgetItem* item, int col);
    void onAddActiveClicked();
    void onRemoveActiveClicked();
    void onActiveListClicked(QListWidgetItem* item);
    void onMouseMoved(const QPointF& pos);
    void onZoomChanged(int percentage);
    void onComponentSelected(Component* comp);
    void onPropertyValueChanged(QTableWidgetItem* item);
    void onMusicSelected(const QString& songName);

    // 🌟 اسلات‌های جدید کنترل شبیه‌سازی و تایمر
    void onRunSimulation();
    void onPauseSimulation();
    void onStopSimulation();
    void onUpdateSimTimer();

private:
    void initWorkspaceWidgets();
    void createSimulationToolBar(); // 🌟 ساخت نوار ابزار شبیه‌سازی

    QStackedWidget *stackedWidget;
    StartMenu *startMenu;
    MainCanvas *mainCanvas;

    QDockWidget *libraryDock;
    QLineEdit *searchEdit;
    QTreeWidget *libraryTree;
    QWidget *previewWidgetPlaceholder;
    QPushButton *btnAddActive;
    QPushButton *btnRemoveActive;
    QListWidget *activeList;

    QLabel *coordLabel;
    QLabel *zoomLabel;

    QMediaPlayer *bgMusic;
    QAudioOutput *audioOutput;
    Component *activeComponent;

    // 🌟 اعضای جدید سیستم تایمر و شبیه‌سازی
    QToolBar *simToolBar;
    QPushButton *btnRun;
    QPushButton *btnPause;
    QPushButton *btnStop;
    QLabel *timerDisplayLabel;
    QTimer *simTimer;
    int elapsedSeconds;
};

#endif // MAINWINDOW_H