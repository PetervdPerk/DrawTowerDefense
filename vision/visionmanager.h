#ifndef VISIONMANAGER_H
#define VISIONMANAGER_H

#include <QObject>
#include <QSlider>
#include <QHash>
#include "../mainwindow.h"

#include "vision/Buffer.h"
#include "vision/capturethread.h"
#include "vision/imagebuffer.h"
#include "vision/processingthread.h"
#include "vision/visionview.h"
#include "vision/task/glyphprocesstask.h"
#include "vision/task/boundingrecttask.h"
#include "vision/task/lineprocesstask.h"
#include "vision/task/markerprocesstask.h"

namespace vision
{
class visionManager : public QObject
{
    Q_OBJECT
public:
    explicit visionManager(MainWindow *window, QObject *parent = 0);
    virtual ~visionManager();
    void enableView();

signals:
    void glyphLoc(QPointF loc, int id);
    void lineDetected(QPolygonF);
    void startDraw();

public slots:
    void setROI(QRect roi);
    void lineFound(QPolygonF);
    void glyphLocSlot(QPointF loc,int id);
    void forceStartGame();
    void switchDetectMethod();

private:
    vision::ImageBuffer* buffer;
    vision::CaptureThread* capture;
    vision::ProcessingThread* proc;
    MainWindow *view;
    vision::task::glyphProcessTask *glyphTask;
    vision::task::markerProcessTask *markerTask;

    vision::task::lineprocesstask *lineTask;
    vision::task::boundingRectTask *rectTask;

    QRect roi;

    QHash<int, QPointF> glyphs;
};
}

#endif // VISIONMANAGER_H
