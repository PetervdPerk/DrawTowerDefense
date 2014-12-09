#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vision/glyphdetector.h"

#include "vision/visionmanager.h"
#include "vision/visionview.h"

#include <QResizeEvent>

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

    vision::visionManager *mgr = new vision::visionManager(this);

    mgr->enableView();

    QObject::connect(mgr, SIGNAL(glyphLoc(QPointF,int)), gm , SLOT(glyphLoc(QPointF,int)));
    QObject::connect(mgr, SIGNAL(lineDetected(QPolygonF)), gm , SLOT(drawLine(QPolygonF)));
    QObject::connect(mgr, SIGNAL(startDraw()), gm, SLOT(startDraw()));

    //detect->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *) {
#ifndef YOCTO
    gameView->resizeHandler();
#endif
}

void MainWindow::updateFrame(const QImage &frame){
    ui->view->setPixmap(QPixmap::fromImage(frame));
}

void MainWindow::addLayoutToVBOX(QLayout *layout){
    ui->vbox->addLayout(layout);
}

void MainWindow::addLayoutToVBOX2(QLayout *layout){
    ui->vbox2->addLayout(layout);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    QSize tempSize=ui->graphicsView->size();
    //ui->graphicsView->resizeEvent(new QResizeEvent(tempSize,tempSize));
    //ui->tabGame->resizeEvent(new QResizeEvent(tempSize,tempSize));
    QMainWindow::resizeEvent(new QResizeEvent(tempSize,tempSize));
}
