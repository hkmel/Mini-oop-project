#include "startmenu.h"
#include <QDialog>
#include <QRadioButton>
#include <QDialogButtonBox>

StartMenu::StartMenu(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadRecentProjects();
}

StartMenu::~StartMenu() {}

void StartMenu::setupUI()
{
    setMinimumSize(750, 550);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(45, 50, 45, 50);
    mainLayout->setSpacing(25);

    titleLabel = new QLabel("Welcome to Proteus Simulator", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "font-size: 32px;"
        "font-weight: bold;"
        "color: #4CAF50;"
        "background: transparent;"
        );
    mainLayout->addWidget(titleLabel);

    QWidget *centerBox = new QWidget(this);
    centerBox->setObjectName("centerBox");
    centerBox->setMaximumWidth(700);

    QHBoxLayout *contentLayout = new QHBoxLayout(centerBox);
    contentLayout->setContentsMargins(25, 25, 25, 25);
    contentLayout->setSpacing(30);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(20);

    QLabel *startTitle = new QLabel("Get Started", centerBox);
    startTitle->setStyleSheet("font-size: 18px; font-weight: bold; background: transparent;");
    leftLayout->addWidget(startTitle);

    newProjectBtn = new QPushButton("Create New Project", centerBox);
    newProjectBtn->setMinimumHeight(50);
    newProjectBtn->setStyleSheet(
        "QPushButton{"
        "background: #4CAF50;"
        "color: white;"
        "border-radius: 10px;"
        "font-size: 15px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover{"
        "background: #66BB6A;"
        "}"
        );
    connect(newProjectBtn, &QPushButton::clicked, this, &StartMenu::onNewProjectClicked);
    leftLayout->addWidget(newProjectBtn);

    openProjectBtn = new QPushButton("Open Existing Project", centerBox);
    openProjectBtn->setMinimumHeight(50);
    openProjectBtn->setStyleSheet(
        "QPushButton{"
        "background: #0288D1;"
        "color: white;"
        "border-radius: 10px;"
        "font-size: 15px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover{"
        "background: #03A9F4;"
        "}"
        );
    connect(openProjectBtn, &QPushButton::clicked, this, &StartMenu::openProjectRequested);
    leftLayout->addWidget(openProjectBtn);
    leftLayout->addStretch();

    contentLayout->addLayout(leftLayout, 1);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    QLabel *recentTitle = new QLabel("Recent Projects", centerBox);
    recentTitle->setStyleSheet("font-size: 18px; font-weight: bold; background: transparent;");
    rightLayout->addWidget(recentTitle);

    recentProjectsList = new QListWidget(centerBox);
    recentProjectsList->setMinimumHeight(200);
    connect(recentProjectsList, &QListWidget::currentTextChanged, this, &StartMenu::recentProjectSelected);
    rightLayout->addWidget(recentProjectsList);

    contentLayout->addLayout(rightLayout, 1);

    mainLayout->addWidget(centerBox, 0, Qt::AlignCenter);

    settingsGroup = new QGroupBox("⚙ Settings", this);
    settingsGroup->setMaximumWidth(700);
    QHBoxLayout *settingsLayout = new QHBoxLayout(settingsGroup);

    themeLabel = new QLabel("Theme:", this);
    themeLabel->setStyleSheet("font-size: 15px; font-weight: bold; background: transparent;");

    themeComboBox = new QComboBox(this);
    themeComboBox->addItems(QStringList() << "Dark Mode" << "Ocean Blue");
    themeComboBox->setMinimumWidth(180);
    themeComboBox->setMinimumHeight(35);
    connect(themeComboBox, &QComboBox::currentTextChanged, this, &StartMenu::onThemeChanged);

    settingsLayout->addWidget(themeLabel);
    settingsLayout->addSpacing(20);
    settingsLayout->addWidget(themeComboBox);
    settingsLayout->addStretch();

    mainLayout->addWidget(settingsGroup, 0, Qt::AlignCenter);

    applyDarkTheme();
}

void StartMenu::loadRecentProjects()
{
    recentProjectsList->addItem("Digital Clock Project.json");
    recentProjectsList->addItem("Counter 8bit Project.json");
    recentProjectsList->addItem("Power Supply Project.json");
}

void StartMenu::onNewProjectClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Create New Project");
    QVBoxLayout layout(&dialog);

    QLabel label("Choose Canvas Size:", &dialog);
    layout.addWidget(&label);

    QRadioButton a4("A4 Standard", &dialog);
    QRadioButton a3("A3 Large", &dialog);
    QRadioButton custom("Custom 1000x1000", &dialog);
    a4.setChecked(true);

    layout.addWidget(&a4);
    layout.addWidget(&a3);
    layout.addWidget(&custom);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout.addWidget(&buttons);

    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString size = a4.isChecked() ? "A4" : (a3.isChecked() ? "A3" : "Custom");
        emit newProjectRequested(size);
    }
}

void StartMenu::onThemeChanged(const QString &theme)
{
    if (theme == "Dark Mode") applyDarkTheme();
    else applyBlueTheme();
}

void StartMenu::applyDarkTheme()
{
    setStyleSheet(
        "StartMenu { background: transparent; }"
        "QWidget#centerBox {"
        "   background: rgba(32, 33, 36, 220);"
        "   border: 1px solid #444;"
        "   border-radius: 16px;"
        "   color: #eeeeee;"
        "   font-family: Arial;"
        "}"
        "QLabel { color: #eeeeee; }"
        "QGroupBox{"
        "   background: rgba(32, 33, 36, 220);"
        "   border: 1px solid #555;"
        "   border-radius: 12px;"
        "   color: #eeeeee;"
        "}"
        "QListWidget{"
        "   background: #151515;"
        "   color: white;"
        "   border-radius: 10px;"
        "   padding: 5px;"
        "}"
        "QComboBox{"
        "   background: #303134;"
        "   color: white;"
        "   border-radius: 8px;"
        "   padding: 5px;"
        "}"
        );
}

void StartMenu::applyBlueTheme()
{
    setStyleSheet(
        "StartMenu { background: transparent; }"
        "QWidget#centerBox {"
        "   background: rgba(16, 42, 67, 220);"
        "   border: 1px solid #39739D;"
        "   border-radius: 16px;"
        "   color: white;"
        "   font-family: Arial;"
        "}"
        "QLabel { color: white; }"
        "QGroupBox{"
        "   background: rgba(16, 42, 67, 220);"
        "   border: 1px solid #39739D;"
        "   border-radius: 12px;"
        "   color: white;"
        "}"
        "QListWidget{"
        "   background: #082032;"
        "   color: white;"
        "   border-radius: 10px;"
        "   padding: 5px;"
        "}"
        "QComboBox{"
        "   background: #163A5F;"
        "   color: white;"
        "   border-radius: 8px;"
        "   padding: 5px;"
        "}"
        );
}