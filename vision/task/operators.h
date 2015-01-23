/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2014 HAN Embedded Systems Engineering
 * Author     : Hugo
 *
 * Description: Header file for the implementation of image operators
 *
 ******************************************************************************
  Change History:

    Version 2.0 - October 2012
    > Implemented new image sructure
    > Added platform dependent QDEBUG macro

    Version 1.0 - September 2011
    > Initial revision

******************************************************************************/
#ifndef _OPERATORS_H_
#define _OPERATORS_H_

#include "stdint.h"

// ----------------------------------------------------------------------------
// Debugging macro
// ----------------------------------------------------------------------------
#ifdef QDEBUG_ENABLE

#include <QDebug>
#define QDEBUG(x) qDebug()<<x

// Example usage:
// QDEBUG("Debug " << parameter << " value");

#else

#define QDEBUG(x) //x

#define MAXBLOBS 190

#endif

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
// Image attributes
// Make sure (IMG_HEIGHT * IMG_WIDTH) is a power of 32 (DMA)
#define IMG_HEIGHT  (480)
#define IMG_WIDTH   (640)

// Image displaying LookUp Tables
// These values must match the index values of the Qt dropdownboxes
#define LUT_STRETCH (0)
#define LUT_CLIP    (1)
#define LUT_BINARY  (2)
#define LUT_LABELED (3)


#define PI 3.141592654

// ----------------------------------------------------------------------------
// Type definitions
// ----------------------------------------------------------------------------
typedef uint8_t byte;

typedef struct
{
    uint16_t x_offset;
    uint16_t y_offset;
    uint16_t width;
    uint16_t height;
    uint16_t lut;
    uint16_t dummy; // Make sure the size of this struct is a power of 32 (DMA)
    uint8_t  data[IMG_HEIGHT][IMG_WIDTH];
}image_t;

typedef enum
{
    BRIGHT = 0,
    DARK
}eBrightness;



typedef enum
{
    AND = 0,
    OR = 1
}eOperator;

typedef enum
{
    EQUAL = 0,
    HIGHER = 1
}eMethod;

typedef enum
{
    FOUR  = 4,
    EIGHT = 8
}eConnected;

typedef enum
{
    AVERAGE = 0,
    HARMONIC,
    MAX,
    MEDIAN,
    MIDPOINT,
    MIN,
    RANGE
}eFilterOperation;

typedef struct blobinfo_t
{
    uint16_t height;
    uint16_t width;
    uint16_t nof_pixels;
    float    perimeter;
    uint8_t  corners[4];
    uint8_t  nof_corners;
    uint16_t  min_x;
    uint16_t  max_x;
    uint16_t  min_y;
    uint16_t  max_y;
}blobinfo_t;

typedef struct xy_info
{
    uint16_t  min_x;
    uint16_t  max_x;
    uint16_t  min_y;
    uint16_t  max_y;
}xy_info;

typedef struct { uint16_t x, y; } xy;

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------

int32_t vGetMarkerId(image_t *img);

void vBlobBoundingCornerDetect(image_t *img, xy_info *xy_locs,
                               uint8_t b,
                               blobinfo_t *pBlobInfo);

void vElimate(image_t *src,
              image_t *dst, uint8_t rPixel, uint8_t *kPixels, uint8_t kSize);

void vPerspectiveTransform(image_t *src,image_t *dst,
                           uint8_t dstRectSize, xy quad[4]);

void vRotateDegrees(image_t *src, image_t *dst, double rotation, int x_move, int y_move);

void vInverseWarp(image_t *src, image_t *dst, uint8_t sx, uint8_t sy, uint8_t ex, uint8_t ey,
                  int *Wx, int *Wy);

void vWarp(image_t *src, image_t *dst, uint8_t sx, uint8_t sy, uint8_t ex, uint8_t ey,
           int *Wx, int *Wy);

void vDetermMinMaxXY(image_t *img, xy_info *pXy, const uint8_t blobcount);

// The contrast of the src image is stretch to 0-255 in the dst image
// This function is required by the Qt PC application. It is used to display
// the source- and destination image. If this function is not implemented, the
// user will see random data.
void vContrastStretch(image_t *src, // must be a greyscale image
                      image_t *dst, // result is a greyscale image
                      uint16_t bottom,
                      uint16_t top);

// All pixels in src image with value between 'low' and 'high' are
// set to 1 in dst image. Rest is set to 0
// This function is used in all VisionSets. Without it, initially nothing will
// seem to happen.
void vThreshold(image_t *src,
                image_t *dst, // result is a binary image
                uint8_t low,
                uint8_t high);

// Rotates an image 180 degrees
// This function is required by the microcontroller application. The camera is
// mounted upside down.
void vRotate180(image_t *img);

// This function does the same as vContrastStretch. However, it always
// stretches from 0 to 255 and implements a fast algorithm.
void vContrastStretchFast(image_t *src,  // must be a greyscale image
                          image_t *dst); // result is a greyscale image

// All pixel values are set to 0
void vErase(image_t *img);

void vApplyROI(image_t *img, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height);

// Src and dst image are the same
void vCopy(image_t *src,
           image_t *dst);

// Src and dst image are added pixel by pixel
// Result in dst image
void vAdd(image_t *src,
          image_t *dst);

// Src picture is binary inverted.
// Result in dst image
void vInvert(image_t *src, // must be a binary image
             image_t *dst);

// Src and dst image are multiplied pixel by pixel
// Result in dst image
void vMultiply(image_t *src,
               image_t *dst);

// Src image is copied to dst image
// All pixels in dst image with value 'selected' are set to 'value'
void vSetSelectedToValue(image_t *src,
                         image_t *dst,
                         uint8_t selected,
                         uint8_t value);

void vSetRangeToValue(image_t *src,
                      image_t *dst,
                      uint8_t low,
                      uint8_t high,
                      uint8_t value);

// Src image is copied to dst image
// All border pixels of dst image are set to 'value'
void vSetBorders(image_t *src,
                 image_t *dst,
                 uint8_t value);

// Uses a bitplane (mask) to slice the source image
void vBitSlicing(image_t *src,
                 image_t *dst,
                 uint8_t mask);

// Make a histogram of the source image and calculate the sum of all pixel
// values
void vHistogram(image_t  *src,
                uint16_t *hist,
                uint32_t *sum);

// Threshold values are automatically generated based on the histogram of
// src image
void vThresholdIsoData(image_t *src,
                       image_t *dst,
                       eBrightness brightness); // DARK | BRIGHT

// Threshold values are automatically generated based on the histogram of
// src image
void vThresholdOtsu(image_t *src,
                    image_t *dst,
                    eBrightness brightness); // DARK | BRIGHT

// Fill holes
void vFillHoles(image_t *src, // must be a binary image
                image_t *dst,
                eConnected connected); // FOUR | EIGHT

// Remove the border blobs
void vRemoveBorderBlobs(image_t *src, // must be a binary image
                        image_t *dst,
                        eConnected connected); // FOUR | EIGHT

void vSobel(image_t *src, image_t *dst);

// Find the edges of binary images
void vBinaryEdgeDetect(image_t *src, // must be a binary image
                       image_t *dst,
                       eConnected connected);


xy* fast9_detect(const uint8_t* im, int xsize, int ysize, int stride, int b, int* ret_num_corners);
int vMarkCorners(image_t *src, image_t *dst, xy* corners, int corner_count, blobinfo_t *blobinfo, uint8_t blobcount);
void vClearNeighBours(image_t *src, image_t *dst, int value);

// A set of nonlinear filters
void vNonlinearFilter(image_t *src,
                      image_t *dst,
                      eFilterOperation fo,
                      uint8_t n); // n equals the mask size
// n should be an odd number

// Label all blobs, returns the number of labeled blobs
uint32_t iLabelBlobs(image_t *src, // must be a binary image
                     image_t *dst,
                     eConnected connected);

uint32_t iLabelBlobsMethod2(image_t *src, // must be a binary image
                            image_t *dst,
                            eConnected connected);

uint32_t iLabelBlobsMethod3(image_t *src, // must be a binary image
                            image_t *dst,
                            eConnected connected);


// Analyse blobs
// pBlobInfo points to a blobinfo_t struct declared by the calling program
void vBlobAnalyse(image_t *img,
                  const uint8_t blobnr,
                  blobinfo_t *pBlobInfo);


int iBasicMoment(image_t *img, uint8_t blobnr, int p, int q);
double iCentralMoment(image_t *img, uint8_t blobnr, int p, int q, int xc, int yc);

// Calculates the centroid of a blob
void vCentroid(image_t *img, uint8_t blobnr, uint16_t *xc, uint16_t *yc);

// Calculates the normalized central moments of a blob specified by p and q
// Ref.: Gonzalez, R. (). 11.3.4 Moment Invariants.
//       In Digital Image Processing. pp. 839-842.
//       New Jersey: Pearson Prentice Hall.
double dNormalizedCentralMoments(image_t *img, // must be a binary or labeled image
                                 uint8_t blobnr, // must be '1' if img is binary
                                 int p,
                                 int q);

// LDF
void vLDF(image_t *src, // must be a binary image
          image_t *dst);

void iFillBlob(image_t *img,
               uint16_t x,
               uint16_t y,
               uint8_t value,
               uint8_t fill,
               eConnected connected);

// This function checks the number of pixels around pixel (x,y) and returns the
// number of pixels that equal value.
uint8_t iNeighbourCount(image_t *img,
                        uint16_t x,
                        uint16_t y,
                        uint8_t value,
                        eConnected connected);

uint8_t iNeighboursEqualOrHigher(image_t *img,
                                 uint16_t x,
                                 uint16_t y,
                                 uint8_t value,
                                 eConnected connected);

uint8_t iNeighboursEqualOrLower(image_t *img,
                                uint16_t x,
                                uint16_t y,
                                uint8_t value,
                                eConnected connected);

#endif // _OPERATORS_H_
// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
