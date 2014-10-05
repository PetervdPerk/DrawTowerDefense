#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vision/glyphdetector.h"

#include "vision/visionmanager.h"
#include "vision/visionview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->gameView = new View(this,ui->graphicsView,100,100);

    gm = new game(gameView);
    gui = new gameUI(gameView,gm);

    QObject::connect(gm, SIGNAL(updateUI(UIupdate)), gui, SLOT(update(UIupdate)));

    //glyphDetector* detect = new glyphDetector(false);


    vision::visionManager *mgr = new vision::visionManager();

    mgr->enableView();




    //detect->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *) {
    gameView->resizeHandler();
}
