#include "glyphprocesstask.h"
#include <QDebug>

vision::task::glyphProcessTask::glyphProcessTask(QObject *parent) : abstractProcessTask(parent)
{
}

void vision::task::glyphProcessTask::process(Mat image){

    processedImage = image(currentROI);

    MDetector.detect(processedImage,TheMarkers,TheCameraParameters,TheMarkerSize);

    for (unsigned int i=0;i<TheMarkers.size();i++) {
        QPointF loc;
        float x = TheMarkers[i].getCenter().x;
        float y = TheMarkers[i].getCenter().y;

        loc.setX( x * widthFactor);
        loc.setY( y * heightFactor);
        if(TheMarkers[i].id == 1){
            qDebug() << "X:" << x << " Y:" << y;
           /* qDebug() << "X:" << loc.x() << " Y:" << loc.y() << "Virtual";
            qDebug() << "Width" << currentROI.width << "Width factor" << widthFactor;
*/
            cout << TheMarkers[i]  << endl;
        }
        emit glyphLoc(loc,TheMarkers[i].id);
    }



}

Mat vision::task::glyphProcessTask::getProcessedImage(){
    for (unsigned int i=0;i<TheMarkers.size();i++) {
        TheMarkers[i].draw(processedImage,Scalar(0,0,255),1);
    }
    return processedImage;
}

