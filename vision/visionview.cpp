#include "visionview.h"
#include "ui_visionview.h"

#include <QPixmap>

vision::visionView::visionView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::visionView)
{
    ui->setupUi(this);


}

vision::visionView::~visionView()
{
    delete ui;
}


void vision::visionView::updateFrame(const QImage &frame){
    ui->view->setPixmap(QPixmap::fromImage(frame));
}
