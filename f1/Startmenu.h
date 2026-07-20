#ifndef STARTMENU_H
#define STARTMENU_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QComboBox>

class StartMenu : public QWidget {
    Q_OBJECT

signals:
    void newProjectRequested(const QString& pageSize);
    void openProjectRequested();
    void recentProjectSelected(const QString& projectName);
    // 🌟 سیگنال‌های جدید برای انتقال موزیک و ولوم به MainWindow
    void musicChanged(const QString& songName);
    void volumeChanged(int volume);

public:
    explicit StartMenu(QWidget *parent = nullptr);
    ~StartMenu();

private slots:
    void onNewProjectClicked();
    void onThemeChanged(const QString &theme);
    void onAboutUsClicked(); // 🌟 اسلات باز کردن کادر درباره ما

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