#include "abstractprocesstask.h"

vision::task::abstractProcessTask::abstractProcessTask(QObject *parent) :
    QObject(parent)
{
}

Mat vision::task::abstractProcessTask::getProcessedImage(){
    return processedImage;
}
