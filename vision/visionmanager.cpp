#include "visionmanager.h"
#include <QThread>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

vision::visionManager::visionManager(QObject *parent) : QObject(parent)
{
    buffer = new vision::ImageBuffer();
    buffer->add(0,new vision::Buffer<Mat>(32));

    glyphTask = new vision::task::glyphProcessTask(this);
    rectTask = new vision::task::boundingRectTask(this);
    lineTask = new vision::task::lineprocesstask(this);

    capture = new vision::CaptureThread(buffer,0,true,640,480);
    capture->start();

    proc = new vision::ProcessingThread(buffer,0,rectTask);
    proc->start();

    QObject::connect(glyphTask, SIGNAL(glyphLoc(QPointF,int)), this, SIGNAL(glyphLoc(QPointF,int)) ); //Pass glyphLoc through
    QObject::connect(glyphTask, SIGNAL(glyphLoc(QPointF,int)), this, SLOT(glyphLocSlot(QPointF,int)) );
    QObject::connect(lineTask, SIGNAL(lineDetected(QPolygonF)), this, SLOT(lineFound(QPolygonF)) );
    QObject::connect(rectTask, SIGNAL(roiRect(QRect)), this, SLOT(setROI(QRect)));
}

void vision::visionManager::enableView(){
    view = new vision::visionView();

    QObject::connect(proc, SIGNAL(newFrame(QImage)), view, SLOT(updateFrame(QImage)));

    view->show();

    //Generate sliders for GUI
    QHBoxLayout *cthreshBox = new QHBoxLayout();
    QLabel *cthreshLabel = new QLabel("Line canny");

    QSlider *cthreshSlider = new QSlider(Qt::Orientation::Horizontal);
    cthreshSlider->setMaximum(255);
    cthreshSlider->setMinimum(0);

    QLabel *cthreshValue = new QLabel();
    QObject::connect(cthreshSlider, SIGNAL(valueChanged(int)), cthreshValue, SLOT(setNum(int)) );
    cthreshSlider->setValue(lineTask->getTreshold());

    cthreshBox->addWidget(cthreshLabel);
    cthreshBox->addWidget(cthreshSlider);
    cthreshBox->addWidget(cthreshValue);

    view->addLayoutToVBOX(cthreshBox);

    QHBoxLayout *threshBox = new QHBoxLayout();
    QLabel *threshLabel = new QLabel("Rect treshhold");

    QSlider *threshSlider = new QSlider(Qt::Orientation::Horizontal);
    threshSlider->setMaximum(255);
    threshSlider->setMinimum(0);

    QLabel *threshValue = new QLabel();
    QObject::connect(threshSlider, SIGNAL(valueChanged(int)), threshValue, SLOT(setNum(int)) );
    threshSlider->setValue(rectTask->getTreshold());

    threshBox->addWidget(threshLabel);
    threshBox->addWidget(threshSlider);
    threshBox->addWidget(threshValue);

    view->addLayoutToVBOX(threshBox);

    QHBoxLayout *whiteBox = new QHBoxLayout();
    QLabel *whiteLabel = new QLabel("White balance");

    QSlider *whiteSlider = new QSlider(Qt::Orientation::Horizontal);
    whiteSlider->setMaximum(65535);
    whiteSlider->setMinimum(0);

    QLabel *whiteValue = new QLabel();
    QObject::connect(whiteSlider, SIGNAL(valueChanged(int)), whiteValue, SLOT(setNum(int)) );
    whiteSlider->setValue(capture->getWhiteBalance());

    whiteBox->addWidget(whiteLabel);
    whiteBox->addWidget(whiteSlider);
    whiteBox->addWidget(whiteValue);

    view->addLayoutToVBOX(whiteBox);

    QHBoxLayout *exposureBox = new QHBoxLayout();
    QLabel *exposureLabel = new QLabel("Exposure");

    QSlider *exposureSlider = new QSlider(Qt::Orientation::Horizontal);
    exposureSlider->setMaximum(65535);
    exposureSlider->setMinimum(0);

    QLabel *exposureValue = new QLabel();
    QObject::connect(exposureSlider, SIGNAL(valueChanged(int)), exposureValue, SLOT(setNum(int)) );
    exposureSlider->setValue(capture->getExposure());

    exposureBox->addWidget(exposureLabel);
    exposureBox->addWidget(exposureSlider);
    exposureBox->addWidget(exposureValue);

    view->addLayoutToVBOX(exposureBox);

    QHBoxLayout *btnBox = new QHBoxLayout();
    QPushButton *btnCalibrate = new QPushButton("Calibrate");
    btnBox->addWidget(btnCalibrate);

    view->addLayoutToVBOX(btnBox);

    QObject::connect(btnCalibrate, SIGNAL(clicked()), rectTask, SLOT(setOk()));
    QObject::connect(threshSlider, SIGNAL(valueChanged(int)), rectTask, SLOT(setThreshold(int)) );
    QObject::connect(cthreshSlider, SIGNAL(valueChanged(int)), lineTask, SLOT(setThreshold(int)) );
    QObject::connect(whiteSlider, SIGNAL(valueChanged(int)), capture, SLOT(setWhiteBalance(int)) );
    QObject::connect(exposureSlider, SIGNAL(valueChanged(int)), capture, SLOT(setExposure(int)) );
}

void vision::visionManager::lineFound(QPolygonF line){
    QObject::disconnect(glyphTask, SIGNAL(glyphLoc(QPointF,int)), this, SLOT(glyphLocSlot(QPointF,int)) );
    proc->setProcessTask(glyphTask);
    emit lineDetected(line);
}

void vision::visionManager::setROI(QRect roi){
    this->roi = roi;
    glyphTask->setROI(roi);
    lineTask->setROI(roi);
    proc->setProcessTask(glyphTask);
}

void vision::visionManager::glyphLocSlot(QPointF loc,int id){
    glyphs.insert(id,loc);
    if(glyphs.contains(1) && glyphs.contains(2)){
        proc->setProcessTask(lineTask);
    }
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
