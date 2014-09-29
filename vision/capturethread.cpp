#include "capturethread.h"

CaptureThread::CaptureThread(ImageBuffer *imgBuffer, int deviceNumber,
                             bool dropFrameIfBufferFull, int width, int height) : QThread()
{
    // Save passed parameters
    this->dropFrameIfBufferFull=dropFrameIfBufferFull;
    this->deviceNumber=deviceNumber;
    this->width = width;
    this->height = height;
    this->imgBuffer = imgBuffer;
    // Initialize variables(s)
    doStop=false;
    sampleNumber=0;
    fpsSum=0;
    fps.clear();
}


void CaptureThread::run()
{
    while(true)
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

        // Save capture time
        captureTime=t.elapsed();
        // Start timer (used to calculate capture rate)
        t.start();

        // Synchronize with other streams (if enabled for this stream)
        imgBuffer->sync(deviceNumber);

        // Capture frame (if available)
        if (!cap.grab())
            continue;

        // Retrieve frame
        cap.retrieve(grabbedFrame);
        // Add frame to buffer
        imgBuffer->getByDeviceNumber(deviceNumber)->add(grabbedFrame, dropFrameIfBufferFull);

        // Update statistics
        updateFPS(captureTime);
        //statsData.nFramesProcessed++;
        // Inform GUI of updated statistics
        //emit updateStatisticsInGUI(statsData);
    }
    qDebug() << "Stopping capture thread...";
}

void CaptureThread::updateFPS(int timeElapsed)
{
    // Add instantaneous FPS value to queue
    if(timeElapsed>0)
    {
        fps.enqueue((int)1000/timeElapsed);
        // Increment sample number
        sampleNumber++;
    }
    // Maximum size of queue is DEFAULT_CAPTURE_FPS_STAT_QUEUE_LENGTH
    if(fps.size()>CAPTURE_FPS_STAT_QUEUE_LENGTH)
        fps.dequeue();
    // Update FPS value every DEFAULT_CAPTURE_FPS_STAT_QUEUE_LENGTH samples
    if((fps.size()==CAPTURE_FPS_STAT_QUEUE_LENGTH)&&(sampleNumber==CAPTURE_FPS_STAT_QUEUE_LENGTH))
    {
        // Empty queue and store sum
        while(!fps.empty())
            fpsSum+=fps.dequeue();
        // Calculate average FPS
        AverageFps=fpsSum/CAPTURE_FPS_STAT_QUEUE_LENGTH;
        // Reset sum
        fpsSum=0;
        // Reset sample number
        sampleNumber=0;
    }
}

void CaptureThread::stop()
{
    QMutexLocker locker(&doStopMutex);
    doStop=true;
}

bool CaptureThread::isCameraConnected()
{
    return cap.isOpened();
}

bool CaptureThread::connectToCamera()
{
    // Open camera
    bool camOpenResult = cap.open(deviceNumber);
    // Set resolution
    if(width != -1)
        cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
    if(height != -1)
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);
    // Return result
    return camOpenResult;
}

bool CaptureThread::disconnectCamera()
{
    // Camera is connected
    if(cap.isOpened())
    {
        // Disconnect camera
        cap.release();
        return true;
    }
    // Camera is NOT connected
    else
        return false;
}

int CaptureThread::getCameraWidth()
{
    return cap.get(CV_CAP_PROP_FRAME_WIDTH);
}

int CaptureThread::getCameraHeight()
{
    return cap.get(CV_CAP_PROP_FRAME_HEIGHT);
}


