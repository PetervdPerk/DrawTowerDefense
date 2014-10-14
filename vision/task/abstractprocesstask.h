#ifndef ABSTRACTPROCESSTASK_H
#define ABSTRACTPROCESSTASK_H

#include <QObject>
#include <QRect>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

namespace vision
{
namespace task
{
class abstractProcessTask : public QObject
{
    Q_OBJECT
public:
    explicit abstractProcessTask(QObject *parent = 0);
    virtual void process(Mat image) = 0;
    virtual Mat getProcessedImage();
    void setROI(QRect roi);

signals:

public slots:

protected:
    Mat processedImage;
    Rect currentROI;
    float widthFactor;
    float heightFactor;

};
}
}

#endif // ABSTRACTPROCESSTASK_H
