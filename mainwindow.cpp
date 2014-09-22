#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "game.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->gameView = new View(this,ui->graphicsView,100,100);

    game *gm = new game(gameView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *) {
    gameView->resizeHandler();
}
