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


StartMenu::~StartMenu()
{

}


void StartMenu::setupUI()
{
    setMinimumSize(750,550);

    applyDarkTheme();


    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(45,35,45,35);
    mainLayout->setSpacing(25);



    titleLabel = new QLabel("Welcome to Proteus Simulator",this);

    titleLabel->setAlignment(Qt::AlignCenter);

    titleLabel->setStyleSheet(
        "font-size:28px;"
        "font-weight:bold;"
        "color:#4CAF50;"
        );


    mainLayout->addWidget(titleLabel);



    QHBoxLayout *contentLayout = new QHBoxLayout();

    contentLayout->setSpacing(30);



    QVBoxLayout *leftLayout = new QVBoxLayout();

    leftLayout->setSpacing(20);



    QLabel *startTitle = new QLabel("Get Started",this);

    startTitle->setStyleSheet(
        "font-size:18px;"
        "font-weight:bold;"
        );


    leftLayout->addWidget(startTitle);



    newProjectBtn = new QPushButton("Create New Project",this);

    newProjectBtn->setMinimumHeight(50);


    newProjectBtn->setStyleSheet(
        "QPushButton{"
        "background:#4CAF50;"
        "border-radius:10px;"
        "font-size:15px;"
        "font-weight:bold;"
        "}"
        "QPushButton:hover{"
        "background:#66BB6A;"
        "}"
        );


    connect(newProjectBtn,
            &QPushButton::clicked,
            this,
            &StartMenu::onNewProjectClicked);


    leftLayout->addWidget(newProjectBtn);



    openProjectBtn = new QPushButton("Open Existing Project",this);

    openProjectBtn->setMinimumHeight(50);


    openProjectBtn->setStyleSheet(
        "QPushButton{"
        "background:#0288D1;"
        "border-radius:10px;"
        "font-size:15px;"
        "font-weight:bold;"
        "}"
        "QPushButton:hover{"
        "background:#03A9F4;"
        "}"
        );


    connect(openProjectBtn,
            &QPushButton::clicked,
            this,
            &StartMenu::openProjectRequested);


    leftLayout->addWidget(openProjectBtn);



    leftLayout->addStretch();



    contentLayout->addLayout(leftLayout,1);




    QVBoxLayout *rightLayout = new QVBoxLayout();


    QLabel *recentTitle = new QLabel("Recent Projects",this);

    recentTitle->setStyleSheet(
        "font-size:18px;"
        "font-weight:bold;"
        );


    rightLayout->addWidget(recentTitle);



    recentProjectsList = new QListWidget(this);

    recentProjectsList->setMinimumHeight(250);


    connect(recentProjectsList,
            &QListWidget::currentTextChanged,
            this,
            &StartMenu::recentProjectSelected);


    rightLayout->addWidget(recentProjectsList);



    contentLayout->addLayout(rightLayout,1);



    mainLayout->addLayout(contentLayout);




    // SETTINGS PANEL

    settingsGroup = new QGroupBox("⚙ Settings",this);


    settingsGroup->setMinimumHeight(100);



    QHBoxLayout *settingsLayout =
        new QHBoxLayout(settingsGroup);



    themeLabel = new QLabel("Theme:",this);


    themeLabel->setStyleSheet(
        "font-size:15px;"
        "font-weight:bold;"
        );



    themeComboBox = new QComboBox(this);


    themeComboBox->addItems(
        QStringList()
        <<"Dark Mode"
        <<"Ocean Blue"
        );


    themeComboBox->setMinimumWidth(180);

    themeComboBox->setMinimumHeight(35);



    connect(themeComboBox,
            &QComboBox::currentTextChanged,
            this,
            &StartMenu::onThemeChanged);



    settingsLayout->addWidget(themeLabel);

    settingsLayout->addSpacing(20);

    settingsLayout->addWidget(themeComboBox);

    settingsLayout->addStretch();



    mainLayout->addWidget(settingsGroup);

}




void StartMenu::loadRecentProjects()
{

    recentProjectsList->addItem(
        "Digital Clock Project.json"
        );

    recentProjectsList->addItem(
        "Counter 8bit Project.json"
        );

    recentProjectsList->addItem(
        "Power Supply Project.json"
        );

}




void StartMenu::onNewProjectClicked()
{

    QDialog dialog(this);

    dialog.setWindowTitle(
        "Create New Project"
        );


    QVBoxLayout layout(&dialog);


    QLabel label(
        "Choose Canvas Size:",
        &dialog
        );


    layout.addWidget(&label);



    QRadioButton a4(
        "A4 Standard",
        &dialog
        );


    QRadioButton a3(
        "A3 Large",
        &dialog
        );


    QRadioButton custom(
        "Custom 1000x1000",
        &dialog
        );


    a4.setChecked(true);



    layout.addWidget(&a4);

    layout.addWidget(&a3);

    layout.addWidget(&custom);



    QDialogButtonBox buttons(
        QDialogButtonBox::Ok |
            QDialogButtonBox::Cancel,
        &dialog
        );


    layout.addWidget(&buttons);



    connect(&buttons,
            &QDialogButtonBox::accepted,
            &dialog,
            &QDialog::accept);


    connect(&buttons,
            &QDialogButtonBox::rejected,
            &dialog,
            &QDialog::reject);



    if(dialog.exec()==QDialog::Accepted)
    {

        QString size;


        if(a4.isChecked())
            size="A4";

        else if(a3.isChecked())
            size="A3";

        else
            size="Custom";


        emit newProjectRequested(size);

    }

}




void StartMenu::onThemeChanged(const QString &theme)
{

    if(theme=="Dark Mode")
        applyDarkTheme();

    else
        applyBlueTheme();

}




void StartMenu::applyDarkTheme()
{

    setStyleSheet(

        "QWidget{"
        "background:#202124;"
        "color:#eeeeee;"
        "font-family:Arial;"
        "}"

        "QGroupBox{"
        "border:1px solid #555;"
        "border-radius:12px;"
        "margin-top:15px;"
        "padding:15px;"
        "}"


        "QListWidget{"
        "background:#151515;"
        "border-radius:10px;"
        "padding:10px;"
        "}"


        "QComboBox{"
        "background:#303134;"
        "color:white;"
        "border-radius:8px;"
        "padding:5px;"
        "}"

        );

}




void StartMenu::applyBlueTheme()
{

    setStyleSheet(

        "QWidget{"
        "background:#102A43;"
        "color:white;"
        "font-family:Arial;"
        "}"


        "QGroupBox{"
        "border:1px solid #39739D;"
        "border-radius:12px;"
        "margin-top:15px;"
        "padding:15px;"
        "}"


        "QListWidget{"
        "background:#082032;"
        "border-radius:10px;"
        "padding:10px;"
        "}"


        "QComboBox{"
        "background:#163A5F;"
        "color:red;"
        "border-radius:8px;"
        "padding:5px;"
        "}"

        );

}