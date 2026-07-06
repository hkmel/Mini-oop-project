#include "startmenu.h"
#include <QMessageBox>

StartMenu::StartMenu(QWidget *parent) : QWidget(parent) {
    setupUI();
    loadRecentProjects();
}

StartMenu::~StartMenu() {}

void StartMenu::setupUI() {
    this->setMinimumSize(600,400);
    this->setStyleSheet("background-color: #2b2b2b; color: #ffffff; font-family: Arial;");

    QVBoxLayout *mainLayout=new QVBoxLayout(this);
    mainLayout->setContentsMargins(40,40,40,40);
    mainLayout->setSpacing(20);

    titleLabel=new QLabel("Welcome to Proteus Simulator",this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #4CAF50;");
    mainLayout->addWidget(titleLabel);

    QHBoxLayout *contentLayout=new QHBoxLayout();

    QVBoxLayout *actionsLayout=new QVBoxLayout();
    actionsLayout->setSpacing(15);

    QLabel *actionsTitle=new QLabel("Get Started:",this);
    actionsTitle->setStyleSheet("font-size: 14px; font-weight: bold;");
    actionsLayout->addWidget(actionsTitle);

    QHBoxLayout *sizeLayout=new QHBoxLayout();
    QLabel *sizeLabel=new QLabel("Canvas Size:", this);
    sizeComboBox=new QComboBox(this);
    sizeComboBox->addItems(QStringList() << "A4 (Standard)" << "A3 (Large)" << "Custom (1000x1000)");
    sizeComboBox->setStyleSheet("background-color: #3e3e3e; padding: 5px; border-radius: 4px;");
    sizeLayout->addWidget(sizeLabel);
    sizeLayout->addWidget(sizeComboBox);
    actionsLayout->addLayout(sizeLayout);

    newProjectBtn=new QPushButton("Create New Project",this);
    newProjectBtn->setStyleSheet("background-color: #4CAF50; padding: 10px; font-weight: bold; border-radius: 5px;");
    connect(newProjectBtn, &QPushButton::clicked, this, &StartMenu::onNewProjectClicked);
    actionsLayout->addWidget(newProjectBtn);

    openProjectBtn = new QPushButton("Open Existing Project", this);
    openProjectBtn->setStyleSheet("background-color: #008CBA; padding: 10px; font-weight: bold; border-radius: 5px;");
    connect(openProjectBtn,&QPushButton::clicked,this,&StartMenu::openProjectRequested);
    actionsLayout->addWidget(openProjectBtn);

    actionsLayout->addStretch();
    contentLayout->addLayout(actionsLayout,1);

    QVBoxLayout *recentLayout = new QVBoxLayout();
    QLabel *recentTitle = new QLabel("Recent Projects:", this);
    recentTitle->setStyleSheet("font-size: 14px; font-weight: bold;");
    recentLayout->addWidget(recentTitle);

    recentProjectsList=new QListWidget(this);
    recentProjectsList->setStyleSheet("background-color: #1e1e1e; border: 1px solid #3e3e3e; border-radius: 5px; padding: 5px;");

    connect(recentProjectsList,&QListWidget::currentTextChanged,this,&StartMenu::recentProjectSelected);
    recentLayout->addWidget(recentProjectsList);

    contentLayout->addLayout(recentLayout,1);
    mainLayout->addLayout(contentLayout);
}

void StartMenu::loadRecentProjects() {
    recentProjectsList->addItem("Project1_DigitalClock.json");
    recentProjectsList->addItem("Project2_Counter8bit.json");
    recentProjectsList->addItem("Project3_PowerSupply.json");
}

void StartMenu::onNewProjectClicked() {
    QString selectedSize = sizeComboBox->currentText();
    emit newProjectRequested(selectedSize);
}