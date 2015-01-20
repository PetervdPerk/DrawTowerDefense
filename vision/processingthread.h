#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H


// Qt
#include <QThread>
#include <QTime>
#include <QQueue>
#include <QImage>
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
// Local
#include "Buffer.h"
#include "imagebuffer.h"
#include "task/abstractprocesstask.h"


// FPS statistics queue lengths
#define PROCESSING_FPS_STAT_QUEUE_LENGTH    32
#define CAPTURE_FPS_STAT_QUEUE_LENGTH       32

using namespace cv;

namespace vision
{
class ProcessingThread : public QThread
{
    Q_OBJECT

public:
    ProcessingThread(ImageBuffer *imgBuffer, int deviceNumber, vision::task::abstractProcessTask *startTask);
    QRect getCurrentROI();
    void stop();
    void setProcessTask(vision::task::abstractProcessTask* task);
    vision::task::abstractProcessTask* getProcessTask(){
        return this->processTask;
    }

public slots:
    void setShowImage(bool show);

private:
    void updateFPS(int);
    QImage MatToQImage(const Mat& mat);
    ImageBuffer *imgBuffer;
    Mat currentFrame;
    Mat currentFrameGrayscale;
    QImage frame;
    QTime t;
    QQueue<int> fps;
    QMutex doStopMutex;
    QMutex processingMutex;
    Size frameSize;
    Point framePoint;
    volatile bool doStop;
    int processingTime;
    int fpsSum;
    int sampleNumber;
    int deviceNumber;
    bool enableFrameProcessing;
    bool showImage = true;

    vision::task::abstractProcessTask *processTask = NULL;

protected:
    void run();

signals:
    void newFrame(const QImage &frame);
};
}


#endif // PROCESSINGTHREAD_H
