#include "processingthread.h"

vision::ProcessingThread::ProcessingThread(ImageBuffer *imgBuffer, int deviceNumber) : QThread()
{
    // Save Device Number
    this->deviceNumber=deviceNumber;
    this->imgBuffer = imgBuffer;
    // Initialize members
    doStop=false;
    sampleNumber=0;
    fpsSum=0;
    fps.clear();
    //statsData.averageFPS=0;
    //statsData.nFramesProcessed=0;
}

QImage vision::ProcessingThread::MatToQImage(const Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    else if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}


void vision::ProcessingThread::run()
{
    while(1)
    {
        ////////////////////////////////
        // Stop thread if doStop=TRUE //
        ////////////////////////////////
        doStopMutex.lock();
        if(doStop)
        {
            doStop=false;
            doStopMutex.unlock();
            break;
        }
        doStopMutex.unlock();
        /////////////////////////////////
        /////////////////////////////////

        // Save processing time
        processingTime=t.elapsed();
        // Start timer (used to calculate processing rate)
        t.start();

        processingMutex.lock();
        // Get frame from queue, store in currentFrame, set ROI
        currentFrame=Mat(imgBuffer->getByDeviceNumber(deviceNumber)->get().clone());


        ////////////////////////////////////
        // PERFORM IMAGE PROCESSING BELOW //
        ////////////////////////////////////



        //index++; //number of images captured
        double tick = (double)getTickCount();//for checking the speed
        //Detection of markers in the image passed
        MDetector.detect(currentFrame,TheMarkers,TheCameraParameters,TheMarkerSize);
        //chekc the speed by calculating the mean speed of all iterations
        AvrgTime.first+=((double)getTickCount()-tick)/getTickFrequency();
        AvrgTime.second++;
        cout<<"\rTime detection="<<1000*AvrgTime.first/AvrgTime.second<<" milliseconds nmarkers="<<TheMarkers.size()<< std::flush;

        //print marker info and draw the markers in image
        currentFrame.copyTo(TheInputImageCopy);

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
       /* if (  TheCameraParameters.isValid())
            for (unsigned int i=0;i<TheMarkers.size();i++) {
                CvDrawingUtils::draw3dCube(TheInputImageCopy,TheMarkers[i],TheCameraParameters);
                CvDrawingUtils::draw3dAxis(TheInputImageCopy,TheMarkers[i],TheCameraParameters);
            }*/
        //DONE! Easy, right?
        //show input with augmented information and  the thresholded image



        ////////////////////////////////////
        // PERFORM IMAGE PROCESSING ABOVE //
        ////////////////////////////////////

        // Convert Mat to QImage
        frame=MatToQImage(TheInputImageCopy);
        processingMutex.unlock();

        // Inform GUI thread of new frame (QImage)
        emit newFrame(frame);

        // Update statistics
        updateFPS(processingTime);
        //statsData.nFramesProcessed++;
        // Inform GUI of updated statistics
        //emit updateStatisticsInGUI(statsData);
    }
    qDebug() << "Stopping processing thread...";
}

void vision::ProcessingThread::updateFPS(int timeElapsed)
{
    // Add instantaneous FPS value to queue
    if(timeElapsed>0)
    {
        fps.enqueue((int)1000/timeElapsed);
        // Increment sample number
        sampleNumber++;
    }

    // Maximum size of queue is DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH
    if(fps.size()>PROCESSING_FPS_STAT_QUEUE_LENGTH)
        fps.dequeue();

    // Update FPS value every DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH samples
    if((fps.size()==PROCESSING_FPS_STAT_QUEUE_LENGTH)&&(sampleNumber==PROCESSING_FPS_STAT_QUEUE_LENGTH))
    {
        // Empty queue and store sum
        while(!fps.empty())
            fpsSum+=fps.dequeue();
        // Calculate average FPS
        //statsData.averageFPS=fpsSum/PROCESSING_FPS_STAT_QUEUE_LENGTH;
        // Reset sum
        fpsSum=0;
        // Reset sample number
        sampleNumber=0;
    }
}

void vision::ProcessingThread::stop()
{
    QMutexLocker locker(&doStopMutex);
    doStop=true;
}
