#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

//QT
#include <QHash>
#include <QSet>
#include <QWaitCondition>
#include <QMutex>
#include <QDebug>

//OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "Buffer.h"

using namespace cv;

class ImageBuffer
{
public:
    ImageBuffer();
    void add(int deviceNumber, Buffer<Mat> *imageBuffer, bool sync=false);
    Buffer<Mat>* getByDeviceNumber(int deviceNumber);
    void removeByDeviceNumber(int deviceNumber);
    void sync(int deviceNumber);
    void wakeAll();
    void setSyncEnabled(bool enable);
    bool isSyncEnabledForDeviceNumber(int deviceNumber);
    bool getSyncEnabled();
    bool containsImageBufferForDeviceNumber(int deviceNumber);

private:
    QHash<int, Buffer<Mat>*> imageBufferMap;
    QSet<int> syncSet;
    QWaitCondition wc;
    QMutex mutex;
    int nArrived;
    bool doSync;

};

#endif // IMAGEBUFFER_H
