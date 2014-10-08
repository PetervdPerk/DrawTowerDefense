#ifndef GLYPHPROCESSTASK_H
#define GLYPHPROCESSTASK_H

#include <QPointF>
#include <QRect>

#include "abstractprocesstask.h"

#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
using namespace aruco;

namespace vision
{
namespace task
{
class glyphProcessTask : public abstractProcessTask
{
    Q_OBJECT
public:
    glyphProcessTask(QObject *parent = 0);
    virtual void process(Mat image);
    virtual Mat getProcessedImage();
    void setROI(QRect roi);

private:
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

    Rect currentROI;
    float widthFactor;
    float heightFactor;

signals:
    void glyphLoc(QPointF,int);
};
}
}

#endif // GLYPHPROCESSTASK_H
