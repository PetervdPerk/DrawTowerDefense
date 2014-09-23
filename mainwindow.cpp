#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->gameView = new View(this,ui->graphicsView,100,100);

    gm = new game(gameView);
    gui = new gameUI(gameView,gm);

    QObject::connect(gm, SIGNAL(updateUI(UIupdate)), gui, SLOT(update(UIupdate)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *) {
    gameView->resizeHandler();
}
