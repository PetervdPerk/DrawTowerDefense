#include "lineprocesstask.h"

vision::task::lineprocesstask::lineprocesstask(QObject *parent) :
    abstractProcessTask(parent), rng(12345)
{
}

void vision::task::lineprocesstask::process(Mat image){

    //image.copyTo(processedImage);
    processedImage = image(currentROI);

#ifndef YOCTO
    cvtColor( processedImage, processedImage, CV_BGR2GRAY );
#endif
    blur( processedImage, processedImage, Size(3,3) );

    /// Detect edges using Threshold
    //threshold( processedImage, processedImage, 128, 255, THRESH_BINARY );
    Canny( processedImage, processedImage, thresh, thresh*ratio, kernel_size );
    int erosion_type = MORPH_CROSS;
    int erosion_size = 1;

    Mat element = getStructuringElement( erosion_type,
                                         Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                         Point( erosion_size, erosion_size ) );

    /// Apply the erosion operation
    dilate( processedImage, processedImage, element );

    //dilate(processedImage,processedImage, Mat());
    //Find contours in the threshold image
    findContours(processedImage, contours, hierarchy, CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);

    // CV_RETR_EXTERNAL
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    // vector<Point2f>center( contours.size() );
    // vector<float>radius( contours.size() );


    int biggestRect = 0;
    int biggestRectId = -1;


    for( int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        //minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
        if(boundRect[i].size().area() > biggestRect) {
            biggestRect = boundRect[i].size().area();
            biggestRectId = i;
        }
    }
    //processedImage.copyTo(drawing);
    drawing = Mat::zeros( processedImage.size(), CV_8UC3 );


    if(biggestRectId != -1){
        int i = biggestRectId;
        Scalar color = Scalar( 255, 0, 0 );
        drawContours( drawing, contours_poly, i, color, 2, 8, vector<Vec4i>(), 0, Point() );
        rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        linePolygon = convertCVPolygonToQTPolygon(contours_poly[i]);
    }

    emit lineDetected(linePolygon);
}

QPolygonF vision::task::lineprocesstask::convertCVPolygonToQTPolygon(vector<Point> cvPolygon){
    QPolygonF polygon;

    for( int i = 0; i < cvPolygon.size(); i++ )
    {
        polygon.append(QPointF((cvPolygon[i].x * widthFactor),(cvPolygon[i].y * heightFactor)));
    }
    return polygon;
}

QPolygonF vision::task::lineprocesstask::getLinePolygon(){
    return linePolygon;
}

Mat vision::task::lineprocesstask::getProcessedImage(){
    return drawing;
}
