#include "boundingrecttask.h"

vision::task::boundingRectTask::boundingRectTask(QObject *parent) :
    abstractProcessTask(parent), rng(12345)
{
}

void vision::task::boundingRectTask::process(Mat image){

    image.copyTo(processedImage);


    cvtColor( processedImage, processedImage, CV_BGR2GRAY );


    blur( processedImage, processedImage, Size(3,3) );

    /// Detect edges using Threshold
    threshold( processedImage, threshold_output, thresh, 255, THRESH_BINARY );
    /// Find contours
    findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    // CV_RETR_EXTERNAL
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    // vector<Point2f>center( contours.size() );
    // vector<float>radius( contours.size() );

    for( int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        //minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
    }


    /// Draw polygonal contour + bonding rects + circles
    //drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
    processedImage.copyTo(drawing);
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
    }

    int biggestRect = 0;
    int biggestRectId;

    for( int i = 0; i< contours.size(); i++ )
    {
        if(boundRect[i].size().area() > biggestRect) {
            biggestRect = boundRect[i].size().area();
            biggestRectId = i;
        }
    }

    //if(biggestRect > 190000){
    if(ok) {
        if(biggestRect > 100000) {
            qDebug() << "Biggest rect " << biggestRect;
            QRect roi(boundRect[biggestRectId].tl().x,
                      boundRect[biggestRectId].tl().y,
                      boundRect[biggestRectId].size().width,
                      boundRect[biggestRectId].size().height);

            emit roiRect(roi);
        }
    }

}

void vision::task::boundingRectTask::setOk(){
    ok = true;
}

Mat vision::task::boundingRectTask::getProcessedImage(){
    //      toggle = !toggle;
    //   if(toggle){
    return drawing;
    //   }

    //return processedImage;
}
