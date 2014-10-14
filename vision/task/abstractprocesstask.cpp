#include "abstractprocesstask.h"

vision::task::abstractProcessTask::abstractProcessTask(QObject *parent) :
    QObject(parent)
{
    currentROI.x = 0;
    currentROI.y = 0;
    currentROI.width = 640;
    currentROI.height = 480;
}

Mat vision::task::abstractProcessTask::getProcessedImage(){
    return processedImage;
}


void vision::task::abstractProcessTask::setROI(QRect roi)
{
    currentROI.x = roi.x();
    currentROI.y = roi.y();
    currentROI.width = roi.width();
    currentROI.height = roi.height();
    widthFactor = (float)(800.00 / currentROI.width);
    heightFactor = (float)(600.00 / currentROI.height);
}
