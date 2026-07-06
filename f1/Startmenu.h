#ifndef STARTMENU_H
#define STARTMENU_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QComboBox>

class StartMenu : public QWidget {
    Q_OBJECT

public:
    explicit StartMenu(QWidget *parent=nullptr);
    ~StartMenu();

signals:
    void newProjectRequested(const QString &pageSize);
    void openProjectRequested();
    void recentProjectSelected(const QString &filePath);

private slots:
    void onNewProjectClicked();

private:
    QLabel *titleLabel;
    QPushButton *newProjectBtn;
    QPushButton *openProjectBtn;
    QComboBox *sizeComboBox;
    QListWidget *recentProjectsList;

    void setupUI();
    void loadRecentProjects();
};

#endif