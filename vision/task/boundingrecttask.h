#ifndef BOUNDINGRECTTASK_H
#define BOUNDINGRECTTASK_H

#include "abstractprocesstask.h"
#include <QDebug>
#include <QRect>

namespace vision
{
namespace task
{
class boundingRectTask : public abstractProcessTask
{
    Q_OBJECT
public:
    explicit boundingRectTask(QObject *parent = 0);
    virtual void process(Mat image);
    virtual Mat getProcessedImage();
    int getTreshold(){
        return thresh;
    }

signals:
    void roiRect(QRect roi);

public slots:
    void setOk();
    void setOkManual();
    void setThreshold(int val) {
        qDebug() << val;
        thresh = val;
    }

private:
    Mat src_gray;
    Mat threshold_output;
    Mat drawing;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    int thresh = 225;
    int max_thresh = 255;
    RNG rng;
    bool toggle = false;

    bool ok = false;

    bool manual = false;


public slots:

};
}
}

#endif // BOUNDINGRECTTASK_H
