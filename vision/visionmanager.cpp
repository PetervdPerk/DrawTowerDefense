#include "visionmanager.h"
#include <QThread>

vision::visionManager::visionManager(QObject *parent) : QObject(parent)
{
    buffer = new vision::ImageBuffer();
    buffer->add(0,new vision::Buffer<Mat>(32));

    capture = new vision::CaptureThread(buffer,0,true,640,480);
    capture->start();

    proc = new vision::ProcessingThread(buffer,0);
    proc->start();

    QObject::connect(proc, SIGNAL(glyphLoc(QPointF,int)), this, SIGNAL(updateTower(QPointF,int)) );

}

void vision::visionManager::enableView(){
    view = new vision::visionView();

    QObject::connect(proc, SIGNAL(newFrame(QImage)), view, SLOT(updateFrame(QImage)));

    view->show();

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
