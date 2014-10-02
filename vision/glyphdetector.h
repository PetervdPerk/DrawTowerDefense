#ifndef GLYPHDETECTOR_H
#define GLYPHDETECTOR_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QDebug>

#include <iostream>
#include <fstream>
#include <sstream>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace aruco;

class glyphDetector : public QThread
{
    Q_OBJECT
public:
    explicit glyphDetector(bool uiEnabled = false, QObject *parent = 0);
    void cvTackBarEvents(int pos,void*);

signals:
    void update();

public slots:
    void updateGUI();

protected:
    void run();

private:
    bool ui;
    QString TheInputVideo;
    QString TheIntrinsicFile;
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



};

#endif // GLYPHDETECTOR_H
