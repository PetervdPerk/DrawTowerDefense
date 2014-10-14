#ifndef LINEPROCESSTASK_H
#define LINEPROCESSTASK_H

#include "abstractprocesstask.h"
#include <QPolygonF>

namespace vision
{
namespace task
{
class lineprocesstask : public abstractProcessTask
{
    Q_OBJECT
public:
    explicit lineprocesstask(QObject *parent = 0);
    virtual void process(Mat image);
    virtual Mat getProcessedImage();
    int getTreshold(){
        return thresh;
    }
    QPolygonF getLinePolygon();

public slots:
    void setThreshold(int val) {
        thresh = val;
    }

signals:
    void lineDetected(QPolygonF line);
    void readyForLineDetect();

public slots:

private:
    QPolygonF convertCVPolygonToQTPolygon(vector<Point>);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    QPolygonF linePolygon;
    Mat drawing;
    int edgeThresh = 1;
    int thresh = 15;
    RNG rng;
    int const max_lowThreshold = 100;
    int ratio = 3;
    int kernel_size = 3;
};
}
}

#endif // LINEPROCESSTASK_H
