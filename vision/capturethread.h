#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H
//QT
#include <QTime>
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QDebug>

//OpenCV
#include <opencv2/highgui/highgui.hpp>

#include "imagebuffer.h"

// FPS statistics queue lengths
#define PROCESSING_FPS_STAT_QUEUE_LENGTH    32
#define CAPTURE_FPS_STAT_QUEUE_LENGTH       32

namespace vision
{
class CaptureThread : public QThread
{
    Q_OBJECT

public:
    CaptureThread(ImageBuffer *imgBuffer, int deviceNumber, bool dropFrameIfBufferFull, int width, int height);
    void stop();
    bool connectToCamera();
    bool disconnectCamera();
    bool isCameraConnected();
    int getCameraWidth();
    int getCameraHeight();

protected:
    void run();

private:
    void updateFPS(int);
    ImageBuffer *imgBuffer;
    VideoCapture cap;
    Mat grabbedFrame;
    QTime t;
    QMutex doStopMutex;
    QQueue<int> fps;
    volatile bool doStop;
    int captureTime;
    int sampleNumber;
    int fpsSum;
    bool dropFrameIfBufferFull;
    int deviceNumber;
    int width;
    int height;
    int AverageFps;



};
}

#endif // CAPTURETHREAD_H
