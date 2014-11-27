#include "processingthread.h"
#include <QDebug>

vision::ProcessingThread::ProcessingThread(ImageBuffer *imgBuffer, int deviceNumber,
                                           vision::task::abstractProcessTask *startTask) : QThread()
{
    // Save Device Number
    this->deviceNumber=deviceNumber;
    this->imgBuffer = imgBuffer;
    // Initialize members
    doStop=false;
    sampleNumber=0;
    fpsSum=0;
    fps.clear();
    this->processTask = startTask;
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


        //cv::flip(currentFrame,currentFrame, 0);
        //cv::flip(currentFrame,currentFrame, 1);
        //Rotate 180 degrees

        processTask->process(currentFrame);

        if(showImage){

            // Convert Mat to QImage
            frame=MatToQImage(processTask->getProcessedImage());

            // Inform GUI thread of new frame (QImage)
            emit newFrame(frame);
        }
        processingMutex.unlock();

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


void vision::ProcessingThread::setShowImage(bool show){
    QMutexLocker locker(&doStopMutex);
    showImage = show;
}

void vision::ProcessingThread::setProcessTask(vision::task::abstractProcessTask* task)
{
    QMutexLocker locker(&doStopMutex);
    this->processTask = task;
}
