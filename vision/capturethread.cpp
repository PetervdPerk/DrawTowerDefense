#include "capturethread.h"

vision::CaptureThread::CaptureThread(ImageBuffer *imgBuffer, int deviceNumber,
                                     bool dropFrameIfBufferFull, int width, int height) : QThread()
  #ifdef YOCTO
  ,cap(jafp::OvVideoCapture::OV_MODE_640_480_15)
  #endif
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

#if defined(UNIX) && !defined(YOCTO)
    devName.append(QString::number(deviceNumber));
    v4l2_fd = v4l2_open(devName.toStdString().c_str(), O_RDWR, 0);
    if (v4l2_fd < 0) {
        qDebug() << "v4l2_open " << devName  << "Open Failed";
    }

    qDebug() << "Get White balance temperature" << v4l2_get_control(v4l2_fd,V4L2_CID_WHITE_BALANCE_TEMPERATURE);
    qDebug() << "Get Exposure absolute" << v4l2_get_control(v4l2_fd,V4L2_CID_EXPOSURE_ABSOLUTE);

    qDebug() << "Set Auto white balance" << v4l2_set_control(v4l2_fd,V4L2_CID_AUTO_WHITE_BALANCE,0);
    qDebug() << "Set White balance temperature" << v4l2_set_control(v4l2_fd,V4L2_CID_WHITE_BALANCE_TEMPERATURE,whiteBalance);
    qDebug() << "Set Exposure Auto" << v4l2_set_control(v4l2_fd,V4L2_CID_EXPOSURE_AUTO,21845);
    qDebug() << "Set Exposure absolute" << v4l2_set_control(v4l2_fd,V4L2_CID_EXPOSURE_ABSOLUTE,exposure);
#endif

#ifdef YOCTO
    if (!cap.open()) {
        qDebug() << "OV5640 capture open failed";
    }

    setExposure(exposure);
    setGain(gain);
    setWB_R(r);
    setWB_G(g);
    setWB_B(b);


#else
    cap.open(0);
#endif
}


int vision::CaptureThread::getWhiteBalance() {
    return whiteBalance;
}

int vision::CaptureThread::getExposure() {
    return exposure;
}

int vision::CaptureThread::getGain() {
    return gain;
}

void vision::CaptureThread::setWhiteBalance(int wb) {
    whiteBalance = wb;
#ifdef YOCTO
    cap.set_control(V4L2_CID_WHITE_BALANCE_TEMPERATURE,whiteBalance);
#elif defined(UNIX)
    v4l2_set_control(v4l2_fd,V4L2_CID_WHITE_BALANCE_TEMPERATURE,whiteBalance);
#else
    qDebug() << "Set white balance to: " << whiteBalance << "not implemented on windows";
#endif
}

void vision::CaptureThread::setExposure(int exp) {
    exposure = exp;
#ifdef YOCTO
    cap.set_control(V4L2_CID_EXPOSURE_ABSOLUTE,exposure);
#elif defined(UNIX)
    v4l2_set_control(v4l2_fd,V4L2_CID_EXPOSURE_ABSOLUTE,exposure);
#else
    qDebug() << "Set exposure to: " << exposure << "not implemented on windows";
#endif
}

void vision::CaptureThread::setGain(int g) {
    gain = g;
#ifdef YOCTO
    cap.set_control(V4L2_CID_GAIN,gain);
#elif defined(UNIX)
    v4l2_set_control(v4l2_fd,V4L2_CID_GAIN,gain);
#else
    qDebug() << "Set exposure to: " << exposure << "not implemented on windows";
#endif
}

void vision::CaptureThread::setWB_R(int r){
    this->r = r;
#ifdef YOCTO
    cap.set_control(V4L2_CID_RED_BALANCE,r);
#endif
}

void vision::CaptureThread::setWB_G(int g){
    this->g = g;
#ifdef YOCTO
    cap.set_control(V4L2_CID_WHITE_BALANCE_TEMPERATURE,g);
#endif
}

void vision::CaptureThread::setWB_B(int b){
    this->b = b;
#ifdef YOCTO
    cap.set_control(V4L2_CID_BLUE_BALANCE,b);
#endif
}

void vision::CaptureThread::run()
{
    qDebug() << "start capture thread";
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
        //updateFPS(captureTime);
        //statsData.nFramesProcessed++;
        // Inform GUI of updated statistics
        //emit updateStatisticsInGUI(statsData);
    }
    qDebug() << "Stopping capture thread...";
}

void vision::CaptureThread::updateFPS(int timeElapsed)
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

void vision::CaptureThread::stop()
{
    QMutexLocker locker(&doStopMutex);
    doStop=true;
}

bool vision::CaptureThread::isCameraConnected()
{
    return cap.isOpened();
}

bool vision::CaptureThread::connectToCamera()
{
    // Open camera
    bool camOpenResult;
#ifdef YOCTO
    camOpenResult = cap.open();
    if (!camOpenResult) {
        qDebug() << "OV5640 capture open failed";
    }
#else
    camOpenResult = cap.open(deviceNumber);
#endif
    // Set resolution
    if(width != -1)
        cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
    if(height != -1)
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);
    // Return result
    return camOpenResult;
}

bool vision::CaptureThread::disconnectCamera()
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

int vision::CaptureThread::getCameraWidth()
{
    return cap.get(CV_CAP_PROP_FRAME_WIDTH);
}

int vision::CaptureThread::getCameraHeight()
{
    return cap.get(CV_CAP_PROP_FRAME_HEIGHT);
}


