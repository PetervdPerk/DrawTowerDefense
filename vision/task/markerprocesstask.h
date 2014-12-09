#ifndef MARKERPROCESSTASK_H
#define MARKERPROCESSTASK_H

#include "abstractprocesstask.h"
#include "opencv2/opencv.hpp"
using namespace cv;

namespace vision
{
namespace task
{
class markerProcessTask : public abstractProcessTask
{
    Q_OBJECT
public:
    markerProcessTask(QObject *parent = 0);
    virtual void process(Mat image);
    virtual Mat getProcessedImage();

public slots:
    void setThreshold(int val) {
        thresh = val;
    }

signals:

public slots:

private:
    Mat processedImage, drawing;
    int thresh = 85;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

signals:
    void glyphLoc(QPointF,int);

};
}
}

#endif // MARKERPROCESSTASK_H
