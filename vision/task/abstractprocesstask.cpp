#include "abstractprocesstask.h"
#include <QDebug>

vision::task::abstractProcessTask::abstractProcessTask(QObject *parent) :
    QObject(parent)
{
    QRect roi;
    roi.setX(0);
    roi.setY(0);
    roi.setWidth(640);
    roi.setHeight(480);
    setROI(roi);
}

Mat vision::task::abstractProcessTask::getProcessedImage(){
    return processedImage;
}


void vision::task::abstractProcessTask::setROI(QRect roi)
{
    qDebug() << "Set ROI";
    currentROI.x = roi.x();
    currentROI.y = roi.y();
    currentROI.width = roi.width();
    currentROI.height = roi.height();
    widthFactor = (float)(800.00 / currentROI.width);
    heightFactor = (float)(560.00 / currentROI.height);
}
