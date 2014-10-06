#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H


// Qt
#include <QThread>
#include <QTime>
#include <QQueue>
#include <QImage>
// OpenCV

#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
using namespace aruco;
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
// Local
#include "Buffer.h"
#include "imagebuffer.h"


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
    ProcessingThread(ImageBuffer *imgBuffer, int deviceNumber);
    QRect getCurrentROI();
    void stop();

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
    float TheMarkerSize=-1;
    int ThePyrDownLevel;
    MarkerDetector MDetector;
    VideoCapture TheVideoCapturer;
    vector<Marker> TheMarkers;
    Mat TheInputImage,TheInputImageCopy;
    CameraParameters TheCameraParameters;

    pair<double,double> AvrgTime;//determines the average time required for detection
    double ThresParam1,ThresParam2;
    int iThresParam1,iThresParam2;
    int waitTime=0;


protected:
    void run();

signals:
    void newFrame(const QImage &frame);
    void glyphLoc(QPointF loc, int id);
};
}


#endif // PROCESSINGTHREAD_H
