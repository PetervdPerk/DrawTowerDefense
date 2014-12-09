#include "markerprocesstask.h"
#include <QGLShaderProgram>

#include <QDebug>

vision::task::markerProcessTask::markerProcessTask(QObject *parent) :
    abstractProcessTask(parent)
{
}


void vision::task::markerProcessTask::process(Mat image){
    processedImage = image(currentROI);

    cvtColor( processedImage, processedImage, CV_BGR2GRAY );

    blur( processedImage, processedImage, Size(3,3) );

    /// Detect edges using Threshold
    threshold( processedImage, processedImage, thresh, 255, THRESH_BINARY );
    /// Find contours
    Mat contourImage;
    processedImage.copyTo(contourImage);
    findContours( contourImage, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );

    RNG rng(12345);

    vector<vector<Point> > contours_poly;
    vector<Point2f> boundRect( contours.size() );
    vector<Point>  approxCurve;

    for( int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( contours[i],approxCurve , double ( contours[i].size() ) *0.05 , true );

        if( approxCurve.size() == 4) //If polygon has 4 points
        {
            if ( isContourConvex ( Mat ( approxCurve ) ) ){
                qDebug() << "Found candidate";
                vector<Point> marker;
                for ( int j=0;j<4;j++ )
                {
                    marker.push_back(Point ( approxCurve[j].x,approxCurve[j].y ) );
                }
                contours_poly.push_back(marker);
                //boundRect.push_back(Point2f ( approxCurve[j].x,approxCurve[j].y ) );
            }
        }
    }

    cvtColor( processedImage, processedImage, CV_GRAY2BGR );
/*
    QGLShader shader(QGLShader::Fragment);
    shader.compileSourceCode(code);

    QGLShaderProgram program(context);
    program.addShader(shader);
    program.link();

    program.bind();*/

    drawing = Mat::zeros( processedImage.size(), CV_8UC3 );
    for( int i = 0; i< contours_poly.size(); i++ )
    {
        //Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        Scalar color = Scalar( 255,255,255 );
        drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
      /*  vector<Point> tmp = contours_poly.at(i);
        const Point* element[1] = { &tmp[0]};
        int nr = (int)tmp.size();
            fillPoly(drawing,element, &nr, 1, color);*/

        //Scalar color2 = Scalar( 128, 40, 80);
        //rectangle( processedImage, boundRect[i].tl(), boundRect[i].br(), color2, 2, 8, 0 );
        //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
    }
}

Mat vision::task::markerProcessTask::getProcessedImage(){
    return processedImage;
}
