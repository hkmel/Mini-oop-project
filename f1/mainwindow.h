#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QDockWidget>
#include <QLineEdit>
#include <QTreeWidget>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include "startmenu.h"
#include "maincanvas.h"
#include <QMediaPlayer>
#include <QAudioOutput>

class Component;
// class ComponentPreviewWidget; // فعلاً کامنت شد

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void handleNewProject(const QString& pageSize);
    void handleOpenProject();
    void onMouseMoved(const QPointF& pos);
    void onZoomChanged(int percentage);
    void onComponentSelected(Component* comp);
    void onPropertyValueChanged(QTableWidgetItem* item);
    void onMusicSelected(const QString& songName); // اسلات مدیریت کلیک آهنگ‌ها
    void filterLibrary(const QString& text);
    void onTreeItemClicked(QTreeWidgetItem* item, int col);
    void onAddActiveClicked();
    void onRemoveActiveClicked();
    void onActiveListClicked(QListWidgetItem* item);

private:
    void initWorkspaceWidgets();

    QStackedWidget* stackedWidget;
    StartMenu* startMenu;
    MainCanvas* mainCanvas;

    QDockWidget* libraryDock;
    QLineEdit* searchEdit;
    QTreeWidget* libraryTree;
    QMediaPlayer* bgMusic = nullptr;     // پخش‌کننده صوتی
    QAudioOutput* audioOutput = nullptr;
    // ComponentPreviewWidget* previewWidget; // فعلاً کامنت شد
    QWidget* previewWidgetPlaceholder; // جایگزین موقت برای چیدمان پنل

    QListWidget* activeList;
    QPushButton* btnAddActive;
    QPushButton* btnRemoveActive;

    QLabel* coordLabel;
    QLabel* zoomLabel;

    Component* activeComponent;
};

#endif // MAINWINDOW_H