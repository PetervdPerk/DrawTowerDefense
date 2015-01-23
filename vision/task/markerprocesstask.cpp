#include "markerprocesstask.h"
#include <QGLShaderProgram>

#include <QDebug>

vision::task::markerProcessTask::markerProcessTask(QObject *parent) :
    abstractProcessTask(parent)
{
}


void vision::task::markerProcessTask::process(Mat image){


   /* cvtColor( processedImage, processedImage, CV_BGR2GRAY );

    blur( processedImage, processedImage, Size(3,3) );

    image.convertTo();*/

    QString info = "";

    cvtColor( image, image, CV_BGR2GRAY );

    pSrc.width = image.cols;
    pSrc.height = image.rows;
    pDst.width = image.cols;
    pDst.height = image.rows;

    //data copy using memcpy function
    memcpy(&pSrc.data[0][0], image.data, sizeof(unsigned char)*pSrc.width*pSrc.height);

    qDebug() << "ROI: " << currentROI.x << currentROI.y << currentROI.width << currentROI.height;

    vApplyROI(&pSrc,currentROI.x,currentROI.y,currentROI.width,currentROI.height);

    // Contrast stretch the source image, result in destination image
    vThresholdIsoData(&pSrc,&pDst,DARK);

    setProcessedImage(&pSrc);
    vSetSelectedToValue(&pView,&pView,0,255);

    vRemoveBorderBlobs(&pDst,&pDst,FOUR);

    //vFillHoles(&pDst,&pDst,FOUR);

    blobcount = iLabelBlobs(&pDst,&pDst,FOUR);



    vBlobAnalyse(&pDst,blobcount,blobinfo);
    int b = 0;
    for(int i = 0; i < blobcount; i++){
        if(blobinfo[i].nof_pixels > 100 && blobinfo[i].width > 15 && blobinfo[i].height > 15){
            vSetSelectedToValue(&pDst,&pDst,i+1,b+1);
            blobinfo[b] = blobinfo[i];
            b++;
        } else {
            vSetSelectedToValue(&pDst,&pDst,i+1,0);
        }
    }
    blobcount = b;
    vSetRangeToValue(&pDst,&pDst,b,255,0);


    /*
    //Method 1
    int corner_count;
    xy *corners = fast9_detect(&pDst.data[0][0],pDst.width,pDst.height,pDst.width,0,&corner_count);
    info.append(QString("6. Corners %1\n").arg(corner_count));

    int label_corners = vMarkCorners(&pDst,&pDst,corners,corner_count,blobinfo,b);
    info.append(QString("7. Marked corners %1\n").arg(label_corners));

    setProcessedImage(&pDst);
    vSetSelectedToValue(&pView,&pView,0,255);
    vSetSelectedToValue(&pView,&pView,1,128);
    vSetSelectedToValue(&pView,&pView,2,128);

    xy centroids[blobcount];
    xy_info corner_loc[label_corners];
    vDetermMinMaxXY(&pDst,&corner_loc[0],label_corners);

    for(int b = 0 ; b < blobcount; b++){
        if(blobinfo[b].nof_corners == 4){
            vCentroid(&pDst,b+1,&centroids[b].x,&centroids[b].y);
        }
    }

    qDebug() << blobcount;
    for(int b = 0 ; b < blobcount; b++){
        if(blobinfo[b].nof_corners == 4){
            xy quad[4];
            for(int n = 0 ; n < 4; n++){
                if(corner_loc[blobinfo[b].corners[n]-1].min_x < centroids[b].x && corner_loc[blobinfo[b].corners[n]-1].min_y < centroids[b].y){
                    quad[0].x = corner_loc[blobinfo[b].corners[n]-1].min_x;
                    quad[0].y = corner_loc[blobinfo[b].corners[n]-1].min_y;
                } else if(corner_loc[blobinfo[b].corners[n]-1].max_x > centroids[b].x && corner_loc[blobinfo[b].corners[n]-1].min_y < centroids[b].y){
                    quad[1].x = corner_loc[blobinfo[b].corners[n]-1].max_x;
                    quad[1].y = corner_loc[blobinfo[b].corners[n]-1].min_y;
                } else if(corner_loc[blobinfo[b].corners[n]-1].max_x > centroids[b].x && corner_loc[blobinfo[b].corners[n]-1].max_y > centroids[b].y){
                    quad[2].x = corner_loc[blobinfo[b].corners[n]-1].max_x;
                    quad[2].y = corner_loc[blobinfo[b].corners[n]-1].max_y;
                } else if(corner_loc[blobinfo[b].corners[n]-1].min_x < centroids[b].x && corner_loc[blobinfo[b].corners[n]-1].max_y > centroids[b].y){
                    quad[3].x = corner_loc[blobinfo[b].corners[n]-1].min_x;
                    quad[3].y = corner_loc[blobinfo[b].corners[n]-1].max_y;
                }
            }

            vPerspectiveTransform(&pSrc,&pDst,70,quad);
            //setProcessedImage(&pDst);


            int mId = vGetMarkerId(&pDst);


            std::vector<cv::Point2f> points;
            points.push_back(
                        cv::Point2f(quad[0].x,quad[0].y));
            points.push_back(
                        cv::Point2f(quad[1].x,quad[1].y));
            points.push_back(
                        cv::Point2f(quad[2].x,quad[2].y));
            points.push_back(
                        cv::Point2f(quad[3].x,quad[3].y));
            aruco::Marker marker(points,mId);
            marker.draw(processedImage,Scalar(0,0,255),1);

            qDebug() << b << " Marker " << mId;

            if(mId > 0){
                QPointF loc;
                loc.setX(centroids[b].x);
                loc.setY(centroids[b].y);
                emit glyphLoc(loc,mId);
            }

            info.append(QString("8. Marker \n 1. %1,%2\n 2. %3,%4\n 3. %5,%6\n 4. %7,%8\n").
                        arg(quad[0].x).arg(quad[0].y).arg(quad[1].x).arg(quad[1].y).arg(quad[2].x).arg(quad[2].y).arg(quad[3].x).arg(quad[3].y));
            info.append(QString("8. Marker centroid %1,%2\n").arg(centroids[b].x).arg(centroids[b].y));


        } else {
            qDebug() << "Not enough " << blobinfo[b].nof_corners;
        }
    }
*/
    //Method 2
    //qDebug() << "Blobcount " << blobcount;
    xy_info blobpoints[blobcount][4];
    xy centroids[blobcount];

    for(int b = 0 ; b < blobcount; b++){
        vBlobBoundingCornerDetect(&pDst,&blobpoints[b][0],b+1,&blobinfo[b]);
        if(blobinfo[b].nof_corners == 4){
            vCentroid(&pDst,b+1,&centroids[b].x,&centroids[b].y);
        }
    }

    for(int b = 0 ; b < blobcount; b++){
        if(blobinfo[b].nof_corners == 4){
            xy quad[4] = {0,0,0,0};

            quad[0].x = (blobpoints[b][0].min_x); // + blobpoints[b][0].max_x) / 2;
            quad[0].y = blobpoints[b][0].min_y;

            quad[1].x = blobpoints[b][1].max_x;
            quad[1].y = (blobpoints[b][1].min_y); // + blobpoints[b][1].max_y) / 2;

            quad[2].x = (blobpoints[b][2].max_x); //+ blobpoints[b][2].min_x) / 2;
            quad[2].y = blobpoints[b][2].max_y;

            quad[3].x = blobpoints[b][3].min_x;
            quad[3].y = (blobpoints[b][3].max_y); // + blobpoints[b][3].max_y) / 2;
            /*
            for(int n = 0 ; n < 4; n++){
                if(blobpoints[b][n].min_x <= centroids[b].x && blobpoints[b][n].min_y <= centroids[b].y){
                    quad[0].x = blobpoints[b][n].min_x;
                    quad[0].y = blobpoints[b][n].min_y;
                } else if(blobpoints[b][n].max_x >= centroids[b].x && blobpoints[b][n].min_y <= centroids[b].y){
                    quad[1].x = blobpoints[b][n].max_x;
                    quad[1].y = blobpoints[b][n].min_y;
                } else if(blobpoints[b][n].max_x >= centroids[b].x && blobpoints[b][n].max_y >= centroids[b].y){
                    quad[2].x = blobpoints[b][n].max_x;
                    quad[2].y = blobpoints[b][n].max_y;
                } else if(blobpoints[b][n].min_x <= centroids[b].x && blobpoints[b][n].max_y >= centroids[b].y){
                    quad[3].x = blobpoints[b][n].min_x;
                    quad[3].y = blobpoints[b][n].max_y;
                } else {
                    qDebug() << "Border error";
                }
            }*/

            vPerspectiveTransform(&pSrc,&pDst,70,quad);
            //setProcessedImage(&pDst);

            mId = vGetMarkerId(&pDst);

            //qDebug() << b << " Marker " << mId <<  "X:" << quad[0].x << " Y:" << quad[0].y;

            if(mId > 0 && mId <= 8){
                QPointF loc;

                loc.setX((centroids[b].x - currentROI.x) * widthFactor);
                loc.setY((centroids[b].y - currentROI.y) * heightFactor);
                emit glyphLoc(loc,mId);
                std::vector<cv::Point2f> points;
                points.push_back(
                            cv::Point2f(quad[0].x,quad[0].y));
                points.push_back(
                            cv::Point2f(quad[1].x,quad[1].y));
                points.push_back(
                            cv::Point2f(quad[2].x,quad[2].y));
                points.push_back(
                            cv::Point2f(quad[3].x,quad[3].y));
                aruco::Marker marker(points,mId);
                marker.draw(processedImage,Scalar(0,0,255),1);
            }



            info.append(QString("8. Marker \n 1. %1,%2\n 2. %3,%4\n 3. %5,%6\n 4. %7,%8\n").
                        arg(quad[0].x).arg(quad[0].y).arg(quad[1].x).arg(quad[1].y).arg(quad[2].x).arg(quad[2].y).arg(quad[3].x).arg(quad[3].y));
            info.append(QString("8. Marker centroid %1,%2\n").arg(centroids[b].x).arg(centroids[b].y));


        } else {
            qDebug() << "Not enough " << blobinfo[b].nof_corners;
        }
    }
    /*
    */
}

void vision::task::markerProcessTask::setProcessedImage(image_t *img){
    vCopy(img,&pView);
    pView.height = img->height;
    pView.width = img->width;
    processedImage = Mat(pView.height, pView.width, CV_8UC1, &pView.data[0][0]);
}


Mat vision::task::markerProcessTask::getProcessedImage(){
    return processedImage;
}
