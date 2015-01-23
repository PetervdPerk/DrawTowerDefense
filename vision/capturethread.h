#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H
//QT
#include <QTime>
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QDebug>

#ifdef UNIX
#include <libv4l2.h>
#include <linux/v4l2-controls.h>
#include <fcntl.h>
#endif

#ifdef YOCTO
#include "vision/ov_video_capture.h"
#endif

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
    int getWhiteBalance();
    int getExposure();
    int getGain();

public slots:
    void setWhiteBalance(int wb);
    void setExposure(int exp);
    void setGain(int g);
    void setWB_R(int r);
    void setWB_G(int g);
    void setWB_B(int b);


protected:
    void run();

private:
    void updateFPS(int);
    ImageBuffer *imgBuffer;
#ifdef YOCTO
    jafp::OvVideoCapture cap;
#else
    VideoCapture cap;
#endif
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

#ifdef YOCTO
    int whiteBalance = 21845;
    int exposure = 26;
    int r = 3122,g = 1479,b = 2864;
#else
    int whiteBalance = 21845;
    int exposure = 4169;
    int r,g,b = 0;
#endif

    int gain = 10;

#ifdef UNIX
    int v4l2_fd;
    QString devName = "/dev/video";
#endif



};
}

#endif // CAPTURETHREAD_H
