#ifndef STARTMENU_H
#define STARTMENU_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>

class StartMenu : public QWidget
{
    Q_OBJECT

public:
    explicit StartMenu(QWidget *parent = nullptr);
    ~StartMenu();

signals:
    void newProjectRequested(const QString &pageSize);
    void openProjectRequested();
    void recentProjectSelected(const QString &filePath);

private slots:
    void onNewProjectClicked();
    void onThemeChanged(const QString &theme);

private:

    void setupUI();
    void loadRecentProjects();
    void applyDarkTheme();
    void applyBlueTheme();

    QLabel *titleLabel;

    QPushButton *newProjectBtn;
    QPushButton *openProjectBtn;

    QListWidget *recentProjectsList;

    QGroupBox *settingsGroup;
    QLabel *themeLabel;
    QComboBox *themeComboBox;
};

#endif // STARTMENU_H