#include "mainwindow.h"
#include "componentlibrary.h"
#include "basecomponents.h"
#include <QToolBar>
#include <QHeaderView>
#include <QStatusBar>
#include <QAction>
#include <QCoreApplication>
MainWindow::MainWindow(QWidget* parent):QMainWindow(parent),activeComponent(nullptr){
    this->setWindowTitle("Proteus OOP Simulator");
    this->resize(1200,800);
    stackedWidget=new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    startMenu=new StartMenu(this);
    stackedWidget->addWidget(startMenu);
    mainCanvas=new MainCanvas(this);
    stackedWidget->addWidget(mainCanvas);
    connect(startMenu,&StartMenu::newProjectRequested,this,&MainWindow::handleNewProject);
    connect(startMenu,&StartMenu::openProjectRequested,this,&MainWindow::handleOpenProject);
    stackedWidget->setCurrentWidget(startMenu);
}
MainWindow::~MainWindow(){}
void MainWindow::handleNewProject(const QString& pageSize){
    mainCanvas->setCanvasSize(pageSize);
    stackedWidget->setCurrentWidget(mainCanvas);
    createWorkspaceUI();
}
void MainWindow::handleOpenProject(){
    mainCanvas->setCanvasSize("A4");
    stackedWidget->setCurrentWidget(mainCanvas);
    createWorkspaceUI();
}
void MainWindow::createWorkspaceUI(){
    toolbar=addToolBar("Tools");
    toolbar->setMovable(false);
    QAction* zoomInAct=toolbar->addAction("Zoom In (+)");
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAct,&QAction::triggered,this,[this](){
        if(mainCanvas->getZoomLevel()<3.0){
            mainCanvas->scale(1.15,1.15);
            mainCanvas->zoomChanged(int(mainCanvas->getZoomLevel()*115));
        }
    });
    QAction* zoomOutAct=toolbar->addAction("Zoom Out (-)");
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAct,&QAction::triggered,this,[this](){
        if(mainCanvas->getZoomLevel()>0.3){
            mainCanvas->scale(1.0/1.15,1.0/1.15);
            mainCanvas->zoomChanged(int(mainCanvas->getZoomLevel()*(100.0/1.15)));
        }
    });
    QAction* resetZoomAct=toolbar->addAction("Reset Zoom (100%)");
    connect(resetZoomAct,&QAction::triggered,this,[this](){
        mainCanvas->resetTransform();
        mainCanvas->zoomChanged(100);
    });
    toolbar->addSeparator();
    QAction* rotateAct=toolbar->addAction("Rotate Selected (R)");
    rotateAct->setShortcut(QKeySequence(Qt::Key_R));
    connect(rotateAct,&QAction::triggered,this,[this](){
        QKeyEvent press(QEvent::KeyPress,Qt::Key_R,Qt::NoModifier);
        QCoreApplication::sendEvent(mainCanvas,&press);
    });
    QAction* deleteAct=toolbar->addAction("Delete Selected (Del)");
    deleteAct->setShortcut(QKeySequence::Delete);
    connect(deleteAct,&QAction::triggered,this,[this](){
        QKeyEvent press(QEvent::KeyPress,Qt::Key_Delete,Qt::NoModifier);
        QCoreApplication::sendEvent(mainCanvas,&press);
    });
    libraryDock=new QDockWidget("Library & Active Devices",this);
    libraryDock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    QWidget* libWidget=new QWidget(libraryDock);
    QVBoxLayout* libLayout=new QVBoxLayout(libWidget);
    libLayout->setContentsMargins(2,2,2,2);
    libLayout->setSpacing(2);
    searchEdit=new QLineEdit(libWidget);
    searchEdit->setPlaceholderText("Keywords...");
    libLayout->addWidget(searchEdit);
    libraryTree=new QTreeWidget(libWidget);
    libraryTree->setHeaderHidden(true);
    ComponentLibrary& lib=ComponentLibrary::getInstance();
    for(const QString& cat:lib.getCategories()){
        QTreeWidgetItem* catItem=new QTreeWidgetItem(libraryTree);
        catItem->setText(0,cat);
        for(const QString& type:lib.getComponentsInCategory(cat)){
            QTreeWidgetItem* compItem=new QTreeWidgetItem(catItem);
            compItem->setText(0,type);
        }
    }
    libLayout->addWidget(libraryTree);
    previewWidget=new ComponentPreviewWidget(libWidget);
    previewWidget->setMinimumHeight(120);
    libLayout->addWidget(previewWidget);
    btnAddActive=new QPushButton("Add to Active",libWidget);
    libLayout->addWidget(btnAddActive);
    activeList=new QListWidget(libWidget);
    libLayout->addWidget(activeList);
    btnRemoveActive=new QPushButton("Remove Active",libWidget);
    libLayout->addWidget(btnRemoveActive);
    libraryDock->setWidget(libWidget);
    addDockWidget(Qt::LeftDockWidgetArea,libraryDock);
    connect(searchEdit,&QLineEdit::textChanged,this,&MainWindow::filterLibrary);
    connect(libraryTree,&QTreeWidget::itemClicked,this,&MainWindow::onTreeItemClicked);
    connect(libraryTree,&QTreeWidget::itemDoubleClicked,this,[this](QTreeWidgetItem*,int){onAddActiveClicked();});
    connect(btnAddActive,&QPushButton::clicked,this,&MainWindow::onAddActiveClicked);
    connect(btnRemoveActive,&QPushButton::clicked,this,&MainWindow::onRemoveActiveClicked);
    connect(activeList,&QListWidget::itemClicked,this,&MainWindow::onActiveListClicked);
    propertiesDock=new QDockWidget("Properties Panel",this);
    propertiesDock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    propertiesTable=new QTableWidget(0,2,propertiesDock);
    propertiesTable->setHorizontalHeaderLabels({"Property","Value"});
    propertiesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    propertiesDock->setWidget(propertiesTable);
    addDockWidget(Qt::RightDockWidgetArea,propertiesDock);
    connect(propertiesTable,&QTableWidget::itemChanged,this,&MainWindow::onPropertyValueChanged);
    QStatusBar* bar=statusBar();
    coordLabel=new QLabel("X: 0,Y: 0",this);
    zoomLabel=new QLabel("Zoom: 100%",this);
    helpLabel=new QLabel("Tip: Double click in Library to add; Right click on Canvas to pan.",this);
    bar->addWidget(coordLabel,1);
    bar->addWidget(zoomLabel,1);
    bar->addPermanentWidget(helpLabel,2);
    connect(mainCanvas,&MainCanvas::mousePositionChanged,this,&MainWindow::onMouseMoved);
    connect(mainCanvas,&MainCanvas::zoomChanged,this,&MainWindow::onZoomChanged);
    connect(mainCanvas,&MainCanvas::componentSelected,this,&MainWindow::onComponentSelected);
}
void MainWindow::onMouseMoved(const QPointF& pos){
    coordLabel->setText(QString("X: %1,Y: %2").arg(qRound(pos.x())).arg(qRound(pos.y())));
}
void MainWindow::onZoomChanged(int percentage){
    zoomLabel->setText(QString("Zoom: %1%").arg(percentage));
}
void MainWindow::onComponentSelected(Component* comp){
    propertiesTable->blockSignals(true);
    propertiesTable->setRowCount(0);
    activeComponent=comp;
    if(!comp){
        propertiesTable->blockSignals(false);
        return;
    }
    int row=0;
    propertiesTable->insertRow(row);
    propertiesTable->setItem(row,0,new QTableWidgetItem("ID"));
    propertiesTable->setItem(row,1,new QTableWidgetItem(comp->getId()));
    propertiesTable->item(row,0)->setFlags(Qt::ItemIsEnabled);
    propertiesTable->item(row,1)->setFlags(Qt::ItemIsEnabled);
    row++;
    propertiesTable->insertRow(row);
    propertiesTable->setItem(row,0,new QTableWidgetItem("Type"));
    propertiesTable->setItem(row,1,new QTableWidgetItem(comp->getName()));
    propertiesTable->item(row,0)->setFlags(Qt::ItemIsEnabled);
    propertiesTable->item(row,1)->setFlags(Qt::ItemIsEnabled);
    row++;
    propertiesTable->insertRow(row);
    propertiesTable->setItem(row,0,new QTableWidgetItem("Position"));
    propertiesTable->setItem(row,1,new QTableWidgetItem(QString("%1,%2").arg(comp->getPosition().x()).arg(comp->getPosition().y())));
    propertiesTable->item(row,0)->setFlags(Qt::ItemIsEnabled);
    propertiesTable->item(row,1)->setFlags(Qt::ItemIsEnabled);
    QString name=comp->getName();
    if(name=="Resistor"){
        ResistorComponent* r=dynamic_cast<ResistorComponent*>(comp);
        if(r){
            row++;
            propertiesTable->insertRow(row);
            propertiesTable->setItem(row,0,new QTableWidgetItem("Resistance (Ohm)"));
            propertiesTable->setItem(row,1,new QTableWidgetItem(QString::number(r->getResistance())));
            propertiesTable->item(row,0)->setFlags(Qt::ItemIsEnabled);
        }
    }else if(name=="Capacitor"){
        CapacitorComponent* c=dynamic_cast<CapacitorComponent*>(comp);
        if(c){
            row++;
            propertiesTable->insertRow(row);
            propertiesTable->setItem(row,0,new QTableWidgetItem("Capacitance (F)"));
            propertiesTable->setItem(row,1,new QTableWidgetItem(QString::number(c->getCapacitance())));
            propertiesTable->item(row,0)->setFlags(Qt::ItemIsEnabled);
        }
    }else if(name=="Inductor"){
        InductorComponent* l=dynamic_cast<InductorComponent*>(comp);
        if(l){
            row++;
            propertiesTable->insertRow(row);
            propertiesTable->setItem(row,0,new QTableWidgetItem("Inductance (H)"));
            propertiesTable->setItem(row,1,new QTableWidgetItem(QString::number(l->getInductance())));
            propertiesTable->item(row,0)->setFlags(Qt::ItemIsEnabled);
        }
    }else if(name=="VoltageSource"){
        VoltageSourceComponent* v=dynamic_cast<VoltageSourceComponent*>(comp);
        if(v){
            row++;
            propertiesTable->insertRow(row);
            propertiesTable->setItem(row,0,new QTableWidgetItem("Voltage (V)"));
            propertiesTable->setItem(row,1,new QTableWidgetItem(QString::number(v->getVoltage())));
            propertiesTable->item(row,0)->setFlags(Qt::ItemIsEnabled);
        }
    }
    propertiesTable->blockSignals(false);
}
void MainWindow::onPropertyValueChanged(QTableWidgetItem* item){
    if(!activeComponent)return;
    int row=item->row();
    QString key=propertiesTable->item(row,0)->text();
    QString val=item->text();
    if(key=="Resistance (Ohm)"){
        ResistorComponent* r=dynamic_cast<ResistorComponent*>(activeComponent);
        if(r)r->setResistance(val.toDouble());
    }else if(key=="Capacitance (F)"){
        CapacitorComponent* c=dynamic_cast<CapacitorComponent*>(activeComponent);
        if(c)c->setCapacitance(val.toDouble());
    }else if(key=="Inductance (H)"){
        InductorComponent* l=dynamic_cast<InductorComponent*>(activeComponent);
        if(l)l->setInductance(val.toDouble());
    }else if(key=="Voltage (V)"){
        VoltageSourceComponent* v=dynamic_cast<VoltageSourceComponent*>(activeComponent);
        if(v)v->setVoltage(val.toDouble());
    }
    mainCanvas->viewport()->update();
}
void MainWindow::filterLibrary(const QString& text){
    bool found=false;
    for(int i=0;i<libraryTree->topLevelItemCount();++i){
        QTreeWidgetItem* catItem=libraryTree->topLevelItem(i);
        bool catMatch=catItem->text(0).contains(text,Qt::CaseInsensitive);
        bool hasVis=false;
        for(int j=0;j<catItem->childCount();++j){
            QTreeWidgetItem* compItem=catItem->child(j);
            bool compMatch=compItem->text(0).contains(text,Qt::CaseInsensitive);
            if(catMatch||compMatch){
                compItem->setHidden(false);
                hasVis=true;
                found=true;
            }else{
                compItem->setHidden(true);
            }
        }
        catItem->setHidden(!(catMatch||hasVis));
        if(hasVis)catItem->setExpanded(true);
    }
    if(!found&&!text.isEmpty())helpLabel->setText("No device found!");
    else helpLabel->setText("Search applied.");
}
void MainWindow::onTreeItemClicked(QTreeWidgetItem* item,int col){
    Q_UNUSED(col);
    if(item->childCount()==0)previewWidget->setComponentType(item->text(0));
    else previewWidget->setComponentType("");
}
void MainWindow::onAddActiveClicked(){
    QTreeWidgetItem* item=libraryTree->currentItem();
    if(item&&item->childCount()==0){
        if(activeList->findItems(item->text(0),Qt::MatchExactly).isEmpty()){
            activeList->addItem(item->text(0));
        }
    }
}
void MainWindow::onRemoveActiveClicked(){
    if(activeList->currentItem()){
        delete activeList->currentItem();
        mainCanvas->setActiveComponentType("");
    }
}
void MainWindow::onActiveListClicked(QListWidgetItem* item){
    if(item)mainCanvas->setActiveComponentType(item->text());
}