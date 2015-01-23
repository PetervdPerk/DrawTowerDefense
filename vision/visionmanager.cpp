#include "visionmanager.h"
#include <QThread>
#include <QPushButton>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>

vision::visionManager::visionManager(MainWindow *window, QObject *parent) : QObject(parent)
{
    this->view = window;

    buffer = new vision::ImageBuffer();
    buffer->add(0,new vision::Buffer<Mat>(64));

    glyphTask = new vision::task::glyphProcessTask(this);
    markerTask = new vision::task::markerProcessTask(this);
    rectTask = new vision::task::boundingRectTask(this);
    lineTask = new vision::task::lineprocesstask(this);

    capture = new vision::CaptureThread(buffer,0,true,640,480);
    capture->start();

    proc = new vision::ProcessingThread(buffer,0,rectTask);
    proc->start();

    QObject::connect(glyphTask, SIGNAL(glyphLoc(QPointF,int)), this, SIGNAL(glyphLoc(QPointF,int)) ); //Pass glyphLoc through
    QObject::connect(markerTask, SIGNAL(glyphLoc(QPointF,int)), this, SIGNAL(glyphLoc(QPointF,int)) ); //Pass glyphLoc through
    QObject::connect(glyphTask, SIGNAL(glyphLoc(QPointF,int)), this, SLOT(glyphLocSlot(QPointF,int)) );
    QObject::connect(markerTask, SIGNAL(glyphLoc(QPointF,int)), this, SLOT(glyphLocSlot(QPointF,int)) );
    QObject::connect(lineTask, SIGNAL(lineDetected(QPolygonF)), this, SLOT(lineFound(QPolygonF)) );
    QObject::connect(rectTask, SIGNAL(roiRect(QRect)), this, SLOT(setROI(QRect)));
}

void vision::visionManager::enableView(){
    //view = new vision::visionView();

    QObject::connect(proc, SIGNAL(newFrame(QImage)), view, SLOT(updateFrame(QImage)));

    //view->show();

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

#ifdef YOCTO
    QHBoxLayout *wbBox = new QHBoxLayout();
    QLabel *redLabel = new QLabel("R");
    QLabel *greenLabel = new QLabel("G");
    QLabel *blueLabel = new QLabel("B");

    QSlider *rSlider = new QSlider(Qt::Orientation::Horizontal);
    rSlider->setMaximum(4096);
    rSlider->setMinimum(0);

    QSlider *gSlider = new QSlider(Qt::Orientation::Horizontal);
    gSlider->setMaximum(4096);
    gSlider->setMinimum(0);

    QSlider *bSlider = new QSlider(Qt::Orientation::Horizontal);
    bSlider->setMaximum(4096);
    bSlider->setMinimum(0);

    QLabel *rValue = new QLabel();
    QLabel *gValue = new QLabel();
    QLabel *bValue = new QLabel();
    QObject::connect(rSlider, SIGNAL(valueChanged(int)), rValue, SLOT(setNum(int)) );
    QObject::connect(gSlider, SIGNAL(valueChanged(int)), gValue, SLOT(setNum(int)) );
    QObject::connect(bSlider, SIGNAL(valueChanged(int)), bValue, SLOT(setNum(int)) );

    wbBox->addWidget(redLabel);
    wbBox->addWidget(rSlider);
    wbBox->addWidget(rValue);

    wbBox->addWidget(greenLabel);
    wbBox->addWidget(gSlider);
    wbBox->addWidget(gValue);

    wbBox->addWidget(blueLabel);
    wbBox->addWidget(bSlider);
    wbBox->addWidget(bValue);

    view->addLayoutToVBOX(wbBox);
#else
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
#endif

    QHBoxLayout *exposureBox = new QHBoxLayout();
    QLabel *exposureLabel = new QLabel("Exposure");

    QSlider *exposureSlider = new QSlider(Qt::Orientation::Horizontal);
#ifdef YOCTO
    exposureSlider->setMaximum(255);
#else
    exposureSlider->setMaximum(65535);
#endif
    exposureSlider->setMinimum(0);

    QLabel *exposureValue = new QLabel();
    QObject::connect(exposureSlider, SIGNAL(valueChanged(int)), exposureValue, SLOT(setNum(int)) );
    exposureSlider->setValue(capture->getExposure());

    exposureBox->addWidget(exposureLabel);
    exposureBox->addWidget(exposureSlider);
    exposureBox->addWidget(exposureValue);

    view->addLayoutToVBOX(exposureBox);

    QHBoxLayout *gainBox = new QHBoxLayout();
    QLabel *gainLabel = new QLabel("Gain");

    QSlider *gainSlider = new QSlider(Qt::Orientation::Horizontal);
    gainSlider->setMaximum(1024);
    gainSlider->setMinimum(0);

    QLabel *gainValue = new QLabel();
    QObject::connect(gainSlider, SIGNAL(valueChanged(int)), gainValue, SLOT(setNum(int)) );
    gainSlider->setValue(capture->getGain());

    gainBox->addWidget(gainLabel);
    gainBox->addWidget(gainSlider);
    gainBox->addWidget(gainValue);

    view->addLayoutToVBOX(gainBox);

    QVBoxLayout *btnBox = new QVBoxLayout();

    QPushButton *btnManCalibrate = new QPushButton("Manual Calibrate");
    btnBox->addWidget(btnManCalibrate);

    QPushButton *btnCalibrate = new QPushButton("Auto Calibrate");
    btnBox->addWidget(btnCalibrate);

    QPushButton *btnStart = new QPushButton("Force start game");
    btnBox->addWidget(btnStart);

    QPushButton *btnSwitch = new QPushButton("Switchs method");
    btnBox->addWidget(btnSwitch);

    QCheckBox *imageBox = new QCheckBox("Show video");
    imageBox->setChecked(true);
    QObject::connect(imageBox, SIGNAL(clicked(bool)), proc, SLOT(setShowImage(bool)));
    btnBox->addWidget(imageBox);

    view->addLayoutToVBOX2(btnBox);


    QObject::connect(btnCalibrate, SIGNAL(clicked()), rectTask, SLOT(setOk()));
    QObject::connect(btnManCalibrate, SIGNAL(clicked()), rectTask, SLOT(setOkManual()));
    QObject::connect(btnStart, SIGNAL(clicked()), this, SLOT(forceStartGame()) );
    QObject::connect(btnSwitch, SIGNAL(clicked()), this, SLOT(switchDetectMethod()) );
    QObject::connect(threshSlider, SIGNAL(valueChanged(int)), rectTask, SLOT(setThreshold(int)) );
    QObject::connect(threshSlider, SIGNAL(valueChanged(int)), markerTask, SLOT(setThreshold(int)) );
    QObject::connect(cthreshSlider, SIGNAL(valueChanged(int)), lineTask, SLOT(setThreshold(int)) );
#ifdef YOCTO
    QObject::connect(rSlider, SIGNAL(valueChanged(int)), capture, SLOT(setWB_R(int)) );
    QObject::connect(gSlider, SIGNAL(valueChanged(int)), capture, SLOT(setWB_G(int)) );
    QObject::connect(bSlider, SIGNAL(valueChanged(int)), capture, SLOT(setWB_B(int)) );
#else
    QObject::connect(whiteSlider, SIGNAL(valueChanged(int)), capture, SLOT(setWhiteBalance(int)) );
#endif
    QObject::connect(exposureSlider, SIGNAL(valueChanged(int)), capture, SLOT(setExposure(int)) );
    QObject::connect(gainSlider, SIGNAL(valueChanged(int)), capture, SLOT(setGain(int)) );
}

void vision::visionManager::forceStartGame(){
    QPainterPath path;
    path.moveTo(000,100);
    path.quadTo(100,600,400,400);
    path.quadTo(100,750,750,480);
    path.quadTo(200,50,300,0);
    path.quadTo(200,50,300,0);
    path.quadTo(100,750,750,480);
    path.quadTo(100,600,400,400);
    path.moveTo(000,100);

    lineFound(path.toFillPolygon());
}

void vision::visionManager::switchDetectMethod(){
    if(proc->getProcessTask() == glyphTask){
        proc->setProcessTask(markerTask);
    } else {
        proc->setProcessTask(glyphTask);
    }
}

void vision::visionManager::lineFound(QPolygonF line){
    QObject::disconnect(glyphTask, SIGNAL(glyphLoc(QPointF,int)), this, SLOT(glyphLocSlot(QPointF,int)) );
    QObject::disconnect(markerTask, SIGNAL(glyphLoc(QPointF,int)), this, SLOT(glyphLocSlot(QPointF,int)) );
    //proc->setProcessTask(glyphTask);
    proc->setProcessTask(markerTask);
    emit lineDetected(line);
}

void vision::visionManager::setROI(QRect roi){
    this->roi = roi;
    glyphTask->setROI(roi);
    lineTask->setROI(roi);
    markerTask->setROI(roi);
    proc->setProcessTask(glyphTask);
    emit startDraw();
}

void vision::visionManager::glyphLocSlot(QPointF loc,int id){
    glyphs.insert(id,loc);
    if(glyphs.contains(1) && glyphs.contains(2)){
       // QThread::sleep(1);
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
