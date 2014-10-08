#ifndef VISIONMANAGER_H
#define VISIONMANAGER_H

#include <QObject>
#include <QSlider>

#include "vision/Buffer.h"
#include "vision/capturethread.h"
#include "vision/imagebuffer.h"
#include "vision/processingthread.h"
#include "vision/visionview.h"
#include "vision/task/glyphprocesstask.h"
#include "vision/task/boundingrecttask.h"

namespace vision
{
class visionManager : public QObject
{
    Q_OBJECT
public:
    explicit visionManager(QObject *parent = 0);
    virtual ~visionManager();
    void enableView();

signals:
    void updateTower(QPointF loc, int id);

public slots:
    void setROI(QRect roi);

private:
    vision::ImageBuffer* buffer;
    vision::CaptureThread* capture;
    vision::ProcessingThread* proc;
    vision::visionView *view;
    vision::task::glyphProcessTask *glyphTask;

    vision::task::boundingRectTask *rectTask;
};
}

#endif // VISIONMANAGER_H
