#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "layoutloaders/clocallayoutloader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    cBaseWidget* baseLayout = ui->widget->getBaseLayout();
    baseLayout->setScriptExecutor(&m_ScriptExecutor);
    cBaseWidget* layout = cLocalLayoutLoader().loadLayouts();
    if (layout)
    {
        layout->setFitToParent(true);
        baseLayout->addWidget(layout);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
