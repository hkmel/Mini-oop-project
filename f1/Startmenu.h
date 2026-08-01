#ifndef STARTMENU_H
#define STARTMENU_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QSlider>
#include <QLineEdit>
#include <QSettings>
#include <QFileInfo>

class StartMenu : public QWidget {
    Q_OBJECT

public:
    explicit StartMenu(QWidget *parent = nullptr);
    ~StartMenu();

public slots:
    void addRecentProject(const QString &filePath);

signals:
    void newProjectRequested(const QString &size, const QString &projectName, const QString &savePath);
    void openProjectRequested();
    void recentProjectSelected(const QString &projectPath);
    void musicChanged(const QString &songName);
    void volumeChanged(int volume);

private slots:
    void onNewProjectClicked();
    void onThemeChanged(const QString &theme);
    void onAboutUsClicked();
    void openHelpDialog();
    void onRecentItemClicked(QListWidgetItem *item);
    void openSettingsDialog();
    void filterRecentProjects(const QString &text);

private:
    void setupUI();
    void loadRecentProjects();
    void applyDarkTheme();
    void applyBlueTheme();

    // عناصر اصلی منو
    QLabel *titleLabel;
    QLabel *statusBadge;
    QPushButton *newProjectBtn;
    QPushButton *openProjectBtn;
    QPushButton *helpBtn;
    QPushButton *settingsBtn;
    QPushButton *aboutUsBtn;

    QLineEdit *searchEdit;
    QListWidget *recentProjectsList;
    QWidget *centerBox;

    // وضعیت‌های داخلی تنظیمات
    QString currentTheme;
    QString currentTrack;
    int currentVolume;
};

#endif // STARTMENU_H