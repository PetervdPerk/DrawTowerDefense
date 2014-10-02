#include <QApplication>
#include "glyphdetector.h"


bool readCameraParameters(string TheIntrinsicFile,CameraParameters &CP,Size size);


void cvTackBarEventsTrampoline(int pos,void* ptr){
    glyphDetector* detector = (glyphDetector*)(ptr);
    detector->cvTackBarEvents(pos,ptr);
}

glyphDetector::glyphDetector(bool uiEnabled, QObject *parent) :
    QThread(parent), AvrgTime(0,0)
{
    ui = uiEnabled;

    try
    {
        int vIdx=0;
        qDebug() << "vIdx is" << vIdx;
        //check if the :idx is here
        cout<<"Opening camera index "<<vIdx<<endl;
        TheVideoCapturer.open(vIdx);
        waitTime=10;

        //read first image to get the dimensions
        TheVideoCapturer>>TheInputImage;

        //read camera parameters if passed
        if (TheIntrinsicFile!="") {
            qDebug() <<"camera parameters passed";
            TheCameraParameters.readFromXMLFile(TheIntrinsicFile.toStdString());
            TheCameraParameters.resize(TheInputImage.size());
        }
        //Configure other parameters
        if (ThePyrDownLevel>0)
            MDetector.pyrDown(ThePyrDownLevel);

        //Create gui
        if(ui) {
            cv::namedWindow("thres",1);
            cv::namedWindow("in",1);
            MDetector.getThresholdParams( ThresParam1,ThresParam2);
            MDetector.setCornerRefinementMethod(MarkerDetector::SUBPIX);
            iThresParam1=ThresParam1;
            iThresParam2=ThresParam2;
            cv::createTrackbar("ThresParam1", "in",&iThresParam1, 13, cvTackBarEventsTrampoline, this);
            cv::createTrackbar("ThresParam2", "in",&iThresParam2, 13, cvTackBarEventsTrampoline, this);
        }

        QObject::connect(this, SIGNAL(update()), this, SLOT(updateGUI()), Qt::QueuedConnection);

    } catch (std::exception &ex)

    {
        cout<<"Exception :"<<ex.what()<<endl;
    }
}

void glyphDetector::run() {
    try{

        qDebug() << "Thread started";
        int index=0;
        //capture until press ESC or until the end of the video
        do
        {

            qDebug() << "retrieve image";
            TheVideoCapturer.retrieve( TheInputImage);
            qDebug() << "image received";
            //copy image

            index++; //number of images captured
            double tick = (double)getTickCount();//for checking the speed
            //Detection of markers in the image passed
            MDetector.detect(TheInputImage,TheMarkers,TheCameraParameters,TheMarkerSize);
            //chekc the speed by calculating the mean speed of all iterations
            AvrgTime.first+=((double)getTickCount()-tick)/getTickFrequency();
            AvrgTime.second++;
            cout<<"\rTime detection="<<1000*AvrgTime.first/AvrgTime.second<<" milliseconds nmarkers="<<TheMarkers.size()<< std::flush;

            //print marker info and draw the markers in image
            TheInputImage.copyTo(TheInputImageCopy);

            for (unsigned int i=0;i<TheMarkers.size();i++) {
                cout<<endl<<TheMarkers[i];
                TheMarkers[i].draw(TheInputImageCopy,Scalar(0,0,255),1);
            }
            if (TheMarkers.size()!=0)            cout<<endl;
            //print other rectangles that contains no valid markers
            /**     for (unsigned int i=0;i<MDetector.getCandidates().size();i++) {
              aruco::Marker m( MDetector.getCandidates()[i],999);
              m.draw(TheInputImageCopy,cv::Scalar(255,0,0));
          }*/

            //draw a 3d cube in each marker if there is 3d info
            if (  TheCameraParameters.isValid())
                for (unsigned int i=0;i<TheMarkers.size();i++) {
                    CvDrawingUtils::draw3dCube(TheInputImageCopy,TheMarkers[i],TheCameraParameters);
                    CvDrawingUtils::draw3dAxis(TheInputImageCopy,TheMarkers[i],TheCameraParameters);
                }
            //DONE! Easy, right?
            //show input with augmented information and  the thresholded image

            emit update();

            qDebug() << "loop done";
        }while(TheVideoCapturer.grab());

        qDebug() << "exit";

    } catch (std::exception &ex)

    {
        cout<<"Exception :"<<ex.what()<<endl;
    }
}

void glyphDetector::updateGUI(){
    if(ui) {
        qDebug() << "update gui";
        moveToThread(QApplication::instance()->thread());
        cv::imshow("in",TheInputImageCopy);
        cv::imshow("thres",MDetector.getThresholdedImage());
        moveToThread(this);
    }
}


void glyphDetector::cvTackBarEvents(int pos,void*)
{
    if (iThresParam1<3) iThresParam1=3;
    if (iThresParam1%2!=1) iThresParam1++;
    if (ThresParam2<1) ThresParam2=1;
    ThresParam1=iThresParam1;
    ThresParam2=iThresParam2;
    MDetector.setThresholdParams(ThresParam1,ThresParam2);
    //recompute
    MDetector.detect(TheInputImage,TheMarkers,TheCameraParameters);
    TheInputImage.copyTo(TheInputImageCopy);
    for (unsigned int i=0;i<TheMarkers.size();i++)	TheMarkers[i].draw(TheInputImageCopy,Scalar(0,0,255),1);
    //print other rectangles that contains no valid markers
    /*for (unsigned int i=0;i<MDetector.getCandidates().size();i++) {
        aruco::Marker m( MDetector.getCandidates()[i],999);
        m.draw(TheInputImageCopy,cv::Scalar(255,0,0));
    }*/

    //draw a 3d cube in each marker if there is 3d info
    if (TheCameraParameters.isValid())
        for (unsigned int i=0;i<TheMarkers.size();i++)
            CvDrawingUtils::draw3dCube(TheInputImageCopy,TheMarkers[i],TheCameraParameters);

    cv::imshow("in",TheInputImageCopy);
    cv::imshow("thres",MDetector.getThresholdedImage());
}
