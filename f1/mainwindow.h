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

#include <QToolBar>



#include "startmenu.h"

#include "maincanvas.h"



class Component;



class MainWindow : public QMainWindow {

    Q_OBJECT



public:

    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();



private slots:

    void handleOpenProject();
    void filterLibrary(const QString& text);
    void handleNewProject(const QString &pageSize, const QString &projectName, const QString &savePath);
    void onTreeItemClicked(QTreeWidgetItem* item, int col);
void onFavoriteClicked();
    void onAddActiveClicked();

    void onRemoveActiveClicked();

    void onActiveListClicked(QListWidgetItem* item);

    void onMouseMoved(const QPointF& pos);

    void onZoomChanged(int percentage);

    void onComponentSelected(Component* comp);

    void onPropertyValueChanged(QTableWidgetItem* item);

    void onMusicSelected(const QString& songName);


    void onBackToStartMenuClicked();
    void onToggleLibraryClicked();
    void onSaveProjectClicked();



private:

    void initWorkspaceWidgets();

    void createSimulationToolBar();
    void setupShortcuts();
void showToastNotification(const QString &message);
    QPushButton* btnBackToStart;
    QPushButton* btnOpenProject;
    QPushButton* btnSaveProject;
    QPushButton* btnToggleLibrary;

    QPushButton* btnZoomIn;
    QPushButton* btnZoomOut;
    QPushButton* btnZoomFit;
    QPushButton* btnFavorite;

    QPushButton* btnUndo;
    QPushButton* btnRedo;

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

    QToolBar *simToolBar;



};



#endif // MAINWINDOW_H