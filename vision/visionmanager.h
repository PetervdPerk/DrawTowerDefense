#ifndef VISIONMANAGER_H
#define VISIONMANAGER_H

#include <QObject>

#include "vision/Buffer.h"
#include "vision/capturethread.h"
#include "vision/imagebuffer.h"
#include "vision/processingthread.h"
#include "vision/visionview.h"

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

private:
    vision::ImageBuffer* buffer;
    vision::CaptureThread* capture;
    vision::ProcessingThread* proc;
    vision::visionView *view;
};
}

#endif // VISIONMANAGER_H
