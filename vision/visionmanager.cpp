#include "visionmanager.h"
#include <QThread>
#include <QPushButton>


vision::visionManager::visionManager(QObject *parent) : QObject(parent)
{
    buffer = new vision::ImageBuffer();
    buffer->add(0,new vision::Buffer<Mat>(32));

    glyphTask = new vision::task::glyphProcessTask(this);

    rectTask = new vision::task::boundingRectTask(this);

    capture = new vision::CaptureThread(buffer,0,true,640,480);
    capture->start();

    proc = new vision::ProcessingThread(buffer,0,rectTask);
    proc->start();

    QObject::connect(glyphTask, SIGNAL(glyphLoc(QPointF,int)), this, SIGNAL(updateTower(QPointF,int)) );
    QObject::connect(rectTask, SIGNAL(roiRect(QRect)), this, SLOT(setROI(QRect)));


}

void vision::visionManager::enableView(){
    view = new vision::visionView();

    QObject::connect(proc, SIGNAL(newFrame(QImage)), view, SLOT(updateFrame(QImage)));

    view->show();


    QSlider *slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setMaximum(255);
    slider->setMinimum(0);
    view->addWidgetToVBOX(slider);

    QPushButton *btn = new QPushButton("Calibrate");

    view->addWidgetToVBOX(btn);


    QObject::connect(btn, SIGNAL(clicked()), rectTask, SLOT(setOk()));

    QObject::connect(slider, SIGNAL(valueChanged(int)), rectTask, SLOT(setThreshold(int)) );

}

void vision::visionManager::setROI(QRect roi){
    glyphTask->setROI(roi);
    proc->setProcessTask(glyphTask);
}

vision::visionManager::~visionManager(){
    capture->stop();
    proc->stop();

    while(proc->isRunning() && capture->isRunning()){
        QThread::sleep(1);
    }

    delete capture;
    delete proc;
    delete buffer;
}
