#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vision/glyphdetector.h"

#include "vision/capturethread.h"
#include "vision/imagebuffer.h"

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

    ImageBuffer* buffer = new ImageBuffer();
    CaptureThread* cap = new CaptureThread(buffer,0,true,640,480);


    //detect->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *) {
    gameView->resizeHandler();
}
