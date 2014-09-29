#include "imagebuffer.h"

ImageBuffer::ImageBuffer()
{
    nArrived=0;
    doSync=false;
}

void ImageBuffer::add(int deviceNumber, Buffer<Mat>* imageBuffer, bool sync)
{
    // Device stream is to be synchronized
    if(sync)
    {
        mutex.lock();
        syncSet.insert(deviceNumber);
        mutex.unlock();
    }
    // Add image buffer to map
    imageBufferMap[deviceNumber]=imageBuffer;
}

Buffer<Mat>* ImageBuffer::getByDeviceNumber(int deviceNumber)
{
    return imageBufferMap[deviceNumber];
}

void ImageBuffer::removeByDeviceNumber(int deviceNumber)
{
    // Remove buffer for device from imageBufferMap
    imageBufferMap.remove(deviceNumber);

    // Also remove from syncSet (if present)
    mutex.lock();
    if(syncSet.contains(deviceNumber))
    {
        syncSet.remove(deviceNumber);
        wc.wakeAll();
    }
    mutex.unlock();
}

void ImageBuffer::sync(int deviceNumber)
{
    // Only perform sync if enabled for specified device/stream
    mutex.lock();
    if(syncSet.contains(deviceNumber))
    {
        // Increment arrived count
        nArrived++;
        // We are the last to arrive: wake all waiting threads
        if(doSync && (nArrived==syncSet.size()))
            wc.wakeAll();
        // Still waiting for other streams to arrive: wait
        else
            wc.wait(&mutex);
        // Decrement arrived count
        nArrived--;
    }
    mutex.unlock();
}

void ImageBuffer::wakeAll()
{
    QMutexLocker locker(&mutex);
    wc.wakeAll();
}

void ImageBuffer::setSyncEnabled(bool enable)
{
    doSync=enable;
}

bool ImageBuffer::isSyncEnabledForDeviceNumber(int deviceNumber)
{
    return syncSet.contains(deviceNumber);
}

bool ImageBuffer::getSyncEnabled()
{
    return doSync;
}

bool ImageBuffer::containsImageBufferForDeviceNumber(int deviceNumber)
{
    return imageBufferMap.contains(deviceNumber);
}
