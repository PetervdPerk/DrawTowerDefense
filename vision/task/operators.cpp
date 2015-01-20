/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2014 HAN Embedded Systems Engineering
 * Author     : Hugo
 *
 * Description: Implementation of image operators
 *              Implement these functions only using C (not C++)!!
 *
 ******************************************************************************
  Change History:

    Version 2.0 - October 2012
    > Implemented new image structure

    Version 1.0 - September 2011
    > Initial revision

******************************************************************************/


#include "operators.h"
#include "math.h"
#include <stdlib.h>
#include <QDebug>

//Options


// vContrastStretch lookup table
#define contrLUT
#define no_memset

#define borderBlobsMethod2


#define HISTOGRAM_SIZE 256


// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------
void vContrastStretch(image_t *src, // must be a greyscale image
                      image_t *dst, // result is a greyscale image
                      uint16_t bottom,
                      uint16_t top)
{
    uint32_t lPixel = 255;
    uint32_t hPixel = 0;
    const uint32_t pixels = sizeof(src->data);
    uint8_t *src_pixel = &src->data[0][0];
    uint8_t *dst_pixel = &dst->data[0][0];
#ifdef contrLUT
    uint8_t calcLUT[256];
#endif
    uint32_t i;

    for(i=0; i<pixels; i++){
        if( *++src_pixel < lPixel ){
            lPixel = *src_pixel;
        }
        if( *src_pixel > hPixel ){
            hPixel = *src_pixel;
        }
    }

    float factor = ((float)(top - bottom) / (float)(hPixel - lPixel));
    float rounding = (0.5f + (float)bottom);
    src_pixel = &src->data[0][0];
    float t;

#ifdef contrLUT
    uint8_t *lut = calcLUT;
    uint32_t lSize = 256;
    uint32_t x=0;
    while(lSize--){
        t = x++ - lPixel;
        t *= factor;
        t += rounding;
        *lut++ = t;
    }

    uint32_t iter = pixels;
    while(iter--){
        *dst_pixel++ = calcLUT[*src_pixel++];
    }
#else
    uint32_t iter = pixels;
    while(iter--){
        t = *src_pixel++ - lPixel;
        t *= factor;
        t += rounding;
        *dst_pixel++ = t;
    }
#endif
}

// ----------------------------------------------------------------------------
void vThreshold(image_t *src,
                image_t *dst, // result is a binary image
                uint8_t low,
                uint8_t high)
{
#if __x86_64__ || WIN32
    //qDebug() << "Threshold " << low << " " << high;
#endif
    uint8_t *src_pixel = &src->data[0][0];
    uint8_t *dst_pixel = &dst->data[0][0];
    uint8_t calcLUT[256];
    const uint32_t pixels = sizeof(src->data);
    uint8_t *lut = &calcLUT[256];
    uint32_t iter = 257;

    while(iter--){
        (iter <  low || iter > high) ? ( *lut-- = 0 ):( *lut-- = 1 );
    }
    iter = pixels;
    while(iter--){
        *dst_pixel++ = calcLUT[*src_pixel++];
    }
}

// ----------------------------------------------------------------------------
void vRotate180(image_t *img)
{
    const uint32_t pixels = sizeof(img->data);
    register uint32_t words = (pixels / 8);

    register uint32_t *img_data = (uint32_t*)img->data;
    register uint32_t *img_data_inv = img_data + (words * 2) - 1;
    register uint32_t t;


#ifndef __arm__
#ifdef __win32__
#define __REV(x)    (__bswap_32(x))
#else
#define __REV(x) (0 | ((x & 0x000000ff) << 24) | ((x & 0x0000ff00) << 8) | ((x & 0x00ff0000) >> 8) | ((x & 0xff000000) >> 24))
#endif
#endif

    while(words--){
        t = __REV(*img_data);
        *img_data++ = __REV(*img_data_inv);
        *img_data_inv-- = t;
    }
}

void vContrastStretchFast(image_t *src, // must be a greyscale image
                          image_t *dst) // result is a greyscale image
{
    vContrastStretch(src,dst,0,255);
}

// ----------------------------------------------------------------------------
void vErase(image_t *img)
{
    const uint32_t pixels = sizeof(img->data);
    register uint32_t words = (pixels / 8);

    register uint32_t *img_data = (uint32_t*)img->data;
    const uint32_t black = 0xFFFF;

    while(words--){
        *img_data++ = black;
    }
}

// ----------------------------------------------------------------------------
void vCopy(image_t *src,
           image_t *dst)
{
    const uint32_t pixels = sizeof(src->data);
    register uint32_t words = (pixels / 4);

    register uint32_t *src_data = (uint32_t*)src->data;
    register uint32_t *dst_data = (uint32_t*)dst->data;

    while( words-- )
    {
        *dst_data++ = *src_data++;
    }
}

// ----------------------------------------------------------------------------
void vAdd(image_t *src,
          image_t *dst)
{

#warning TODO: vAdd
    // ********************************************
    // Added to prevent compiler warnings
    // Remove these when implementation starts
    src=src;
    dst=dst;

    return;
    // ********************************************

}

// ----------------------------------------------------------------------------
void vInvert(image_t *src, // Must be a binary image
             image_t *dst)
{
    const uint32_t pixels = sizeof(dst->data);
    register uint32_t bytes = pixels;

    register uint8_t *dst_data = (uint8_t*)dst->data;
    register uint8_t *src_data = (uint8_t*)src->data;

    while(bytes--){
        if(*src_data == 1){
            *dst_data = 0;
        } else {
            *dst_data = 1;
        }
        *src_data++;
        *dst_data++;
    }
}

// ----------------------------------------------------------------------------
void vElimate(image_t *src,
              image_t *dst, uint8_t rPixel, uint8_t *kPixels, uint8_t kSize)
{
    const uint32_t pixels = sizeof(dst->data);
    register uint32_t bytes = pixels;

    register uint8_t *dst_data = (uint8_t*)dst->data;
    register uint8_t *src_data = (uint8_t*)src->data;
    register uint32_t i;
    register uint32_t elimate = 0;

    while(bytes--){
        if(*dst_data == rPixel){
            *dst_data = *src_data;
        } else {
            for(i = 0 ; i <kSize ; i++){
                if(kPixels[i] == *dst_data){
                    elimate = 1;
                    break;
                }
            }
            if(elimate == 0){
                *dst_data = 0xFF;
            } else {
                elimate = 0;
            }
        }

        *src_data++;
        *dst_data++;
    }
}

void vMultiply(image_t *src,
               image_t *dst)
{
    const uint32_t pixels = sizeof(dst->data);
    register uint32_t bytes = pixels;

    register uint8_t *dst_data = (uint8_t*)dst->data;
    register uint8_t *src_data = (uint8_t*)src->data;

    while(bytes--){
        *dst_data = *dst_data * *src_data;
        *src_data++;
        *dst_data++;
    }
}

// ----------------------------------------------------------------------------
void vSetRangeToValue(image_t *src,
                      image_t *dst,
                      uint8_t low,
                      uint8_t high,
                      uint8_t value)
{
    if(src!=dst)
        vCopy(src,dst);

    const uint32_t pixels = sizeof(dst->data);
    register uint32_t bytes = pixels;

    register uint8_t *img_data = (uint8_t*)dst->data;

    while(bytes--){
        if(*img_data > low && *img_data < high){
            *img_data = value;
        }
        *img_data++;
    }
}

void vSetSelectedToValue(image_t *src,
                         image_t *dst,
                         uint8_t selected,
                         uint8_t value)
{
    if(src!=dst)
        vCopy(src,dst);

    const uint32_t pixels = sizeof(dst->data);
    register uint32_t bytes = pixels;

    register uint8_t *img_data = (uint8_t*)dst->data;

    while(bytes--){
        if(*img_data == selected){
            *img_data = value;
        }
        *img_data++;
    }
}

// ----------------------------------------------------------------------------
void vSetBorders(image_t *src,
                 image_t *dst,
                 uint8_t value)
{
    if(src != dst)
        vCopy(src,dst);

    register uint32_t i;

    for(i = 0; i < src->height; i++)
    {
        dst->data[i][0] = value;
        dst->data[i][src->width-1] = value;
        dst->data[0][i] = value;
        dst->data[src->height-1][i] = value;
    }

    for(i = src->height-1; i < src->width; i++)
    {
        dst->data[0][i] = value;
        dst->data[src->height-1][i] = value;
    }
}

// ----------------------------------------------------------------------------
void vBitSlicing(image_t *src,
                 image_t *dst,
                 uint8_t mask)
{

#warning TODO: vBitSlicing
    // ********************************************
    // Added to prevent compiler warnings
    // Remove these when implementation starts
    src=src;
    dst=dst;
    mask=mask;

    return;
    // ********************************************

}

void vHistogramIternal(uint8_t *imgPtr, uint32_t width, uint32_t height,
                       uint16_t *hist,uint32_t *sum){
    int i = HISTOGRAM_SIZE/2;
    uint32_t *hist_data = (uint32_t *)hist;
    while(i--){
        *hist_data++ = 0x0;
    }

    i = height * width;

    *sum = 0;

    while(i--){
        *sum += *imgPtr;
        hist[*imgPtr++]++;
    }
}

// ----------------------------------------------------------------------------
void vHistogram(image_t  *src,
                uint16_t *hist,
                uint32_t *sum)
{
    /* int i = HISTOGRAM_SIZE/2;
    uint32_t *hist_data = (uint32_t *)hist;
    while(i--){
        *hist_data++ = 0x0;
    }

    i = src->height * src->width;

    *sum = 0;
    uint8_t *src_data = &src->data[0][0];

    while(i--){
        *sum += *src_data;
        hist[*src_data++]++;
    }*/

    vHistogramIternal(&src->data[0][0],src->width,src->height,hist,sum);
}

// ----------------------------------------------------------------------------
uint32_t vThresholdIsoDataInternal(uint8_t *imgPtr,uint32_t width, uint32_t height) // DARK | BRIGHT
{
    uint16_t hist[HISTOGRAM_SIZE];
    uint32_t src_total = 0;
    vHistogramIternal(imgPtr,width,height,hist,&src_total);

    uint32_t T = 0, preT = 256, i = 0, Tlow;

    while(hist[T] == 0){
        T++;
    }

    Tlow = T;
    T = 255;

    while(hist[T] == 0){
        T--;
    }

    if(T == Tlow){
        return 0;
    }

    T = Tlow;

    uint32_t meanLeft, meanRight;
    uint32_t hist_sum[HISTOGRAM_SIZE];
    uint32_t hist_px[HISTOGRAM_SIZE];

    hist_sum[0] = 0;
    hist_px[0] = hist[0];

    for(i=1; i<HISTOGRAM_SIZE; i++){
        hist_sum[i] = hist_sum[i-1] + (i * hist[i]);
        hist_px[i] = hist_px[i-1] + hist[i];
    }

    while(T != preT){
        if(hist_px[T] == 0){
            qDebug() << "hist_px[" << T << "] = 0";
            break;
        }
        meanLeft = hist_sum[T] / hist_px[T];
        meanRight = (hist_sum[255] - hist_sum[T]) / (hist_px[255] - hist_px[T]);

        preT = T;
        T = (meanLeft + meanRight)/2;
    }

    return T;
}

void vThresholdIsoData(image_t *src,
                       image_t *dst,
                       eBrightness brightness) // DARK | BRIGHT
{

    uint32_t T = vThresholdIsoDataInternal(&src->data[0][0],src->width,src->height);

    if(brightness == DARK){
        vThreshold(src,dst,0,T);
    }else{
        vThreshold(src,dst,T,0xFF);
    }
}

void vThresholdOtsu(image_t *src, image_t *dst, eBrightness brightness)
{
    uint16_t hist[HISTOGRAM_SIZE] = { 0 };
    uint32_t src_total = 0;
    uint32_t n = src->width*src->height;
    vHistogram(src,hist,&src_total);
    register uint32_t T;

    register uint32_t i = HISTOGRAM_SIZE;
    register uint32_t j = 0;
    register uint32_t max_bcv_loc = 0;

    float   n_obj = 0,
            sum_obj = 0,
            avg_obj = 0,
            n_back = 0,
            sum_back = 0,
            avg_back = 0,
            max_bcv = 0;
    float bcv, u;

    while(i--){

        n_obj += hist[j];
        sum_obj += j * hist[j];

        if(n_obj != 0)
            avg_obj = n_obj / sum_obj;
        else
            avg_obj = 1;

        n_back = n - n_obj;
        sum_back = src_total - sum_obj;

        if(n_back != 0)
            avg_back = n_back / sum_back;
        else
            avg_back = 1;

        u = avg_back / avg_obj;

        bcv = ((n_back * n_obj) * (u*u));
        if(bcv > max_bcv){
            max_bcv = bcv;
            max_bcv_loc = j;
        }
        j++;
    }
    T = max_bcv_loc;
    qDebug() << "T otsu:" << T;
    if(brightness == DARK){
        vThreshold(src,dst,0,T);
    }else{
        vThreshold(src,dst,T,0xFF);
    }
}

// ----------------------------------------------------------------------------
void vFillHoles(image_t *src, // must be a binary image
                image_t *dst,
                eConnected connected) // FOUR | EIGHT
{

    vSetBorders(src,dst,2);


    register uint32_t x,y;

    register uint32_t changed = 1;

    while(changed) {

        changed = 0;
        for(y=1; y < dst->height; y++){
            for(x=1; x < dst->width; x++){
                if(dst->data[y][x] == 0){
                    if(iNeighbourCount(dst,x,y,2,connected) >= 1){
                        dst->data[y][x] = 2;
                        changed = 1;
                    }
                }
            }
        }

        for(y=dst->height-1; y > 0; y--){
            for(x=dst->width-1; x > 0; x--){
                if(dst->data[y][x] == 0){
                    if(iNeighbourCount(dst,x,y,2,connected) >= 1){
                        dst->data[y][x] = 2;
                        changed = 1;
                    }
                }
            }
        }
    }

    vSetSelectedToValue(dst, dst, 0, 1);
    vSetSelectedToValue(dst, dst, 2, 0);

}

// ----------------------------------------------------------------------------
void vRemoveBorderBlobs(image_t *src, // must be a binary image
                        image_t *dst,
                        eConnected connected) // FOUR | EIGHT
{
    vSetBorders(src,dst,2);

    register uint32_t x,y;
    register uint32_t changed = 1;


    while(changed) {

        changed = 0;
        for(y=1; y < dst->height; y++){
            for(x=1; x < dst->width; x++){
                if(dst->data[y][x] == 1){
                    if(iNeighbourCount(dst,x,y,2,connected) >= 1){
                        dst->data[y][x] = 2;
                        changed = 1;
                    }
                }
            }
        }

        for(y=dst->height-1; y > 0; y--){
            for(x=dst->width-1; x > 0; x--){
                if(dst->data[y][x] == 1){
                    if(iNeighbourCount(dst,x,y,2,connected) >= 1){
                        dst->data[y][x] = 2;
                        changed = 1;
                    }
                }
            }
        }
    }

    vSetSelectedToValue(dst, dst, 2, 0);
}




void vSobel(image_t *src, image_t *dst){

    //Sobel filter x and y
    const int32_t horizFilter[3][3] = {{ 1,   0,  -1},
                                       { 2,   0,  -2},
                                       { 1,   0,  -1}};
    const int32_t vertFilter[3][3]  = {{ 1,   2,   1},
                                       { 0,   0,   0},
                                       {-1,  -2,  -1}};

    register uint32_t pixVal, horizPixVal, vertPixVal;
    register uint32_t x, y;
    register int32_t i, j;

    for(x = 1; x < dst->width; x++){
        for(y = 1; y < dst->height; y++){
            pixVal = 0;
            horizPixVal = 0;
            vertPixVal = 0;
            for(i = -1; i <= 1; i++){//Apply filter value for x and y
                for(j = -1; j <= 1; j++){
                    horizPixVal += (int)(src->data[y + j][x + i]) * horizFilter[i + 1][j + 1];
                    vertPixVal  += (int)(src->data[y + j][x + i]) * vertFilter[i + 1][j + 1];
                }
            }

            pixVal = sqrt((horizPixVal * horizPixVal) + (vertPixVal * vertPixVal));

            if(pixVal > 255) pixVal = 255; //if overflow set to max
            dst->data[y][x] = (uint8_t)pixVal;
        }
    }
    vSetBorders(dst,dst,1);
}

// ----------------------------------------------------------------------------
void vBinaryEdgeDetect(image_t *src, // must be a binary image
                       image_t *dst,
                       eConnected connected)
{
    if(src != dst)
        vCopy(src,dst);

    register uint32_t x,y;


    for(y=1; y < dst->height; y++){
        for(x=1; x < dst->width; x++){
            if(dst->data[y][x] == 1){
                if(iNeighboursEqualOrHigher(dst,x,y,1,connected) >= connected){
                    dst->data[y][x] = 2;
                }
            }
        }
    }

    vSetSelectedToValue(dst, dst, 2, 0);
}


static void make_offsets(int pixel[], int row_stride)
{
    pixel[0] = 0 + row_stride * 3;
    pixel[1] = 1 + row_stride * 3;
    pixel[2] = 2 + row_stride * 2;
    pixel[3] = 3 + row_stride * 1;
    pixel[4] = 3 + row_stride * 0;
    pixel[5] = 3 + row_stride * -1;
    pixel[6] = 2 + row_stride * -2;
    pixel[7] = 1 + row_stride * -3;
    pixel[8] = 0 + row_stride * -3;
    pixel[9] = -1 + row_stride * -3;
    pixel[10] = -2 + row_stride * -2;
    pixel[11] = -3 + row_stride * -1;
    pixel[12] = -3 + row_stride * 0;
    pixel[13] = -3 + row_stride * 1;
    pixel[14] = -2 + row_stride * 2;
    pixel[15] = -1 + row_stride * 3;
}

int vMarkCorners(image_t *src, image_t *dst, xy* corners, int corner_count, blobinfo_t *blobinfo, uint8_t blobcount){
    if(src != dst)
        vCopy(src,dst);

    int label = blobcount + 1;
    int blobnr;

    for(int i = 0; i < corner_count; i++){
        if(dst->data[corners[i].y][corners[i].x] != 0) {
            dst->data[corners[i].y][corners[i].x] = 255 - dst->data[corners[i].y][corners[i].x];
        }
    }

    for(int i = 0; i < corner_count; i++){

        blobnr = 255 - dst->data[corners[i].y][corners[i].x];
        if(dst->data[corners[i].y][corners[i].x] > blobnr){
            iFillBlob(dst,corners[i].x,corners[i].y,dst->data[corners[i].y][corners[i].x],label,EIGHT);

            if(dst->data[corners[i].y][corners[i].x] == 255 - blobnr){
                dst->data[corners[i].y][corners[i].x] = 0;
            } else {
                if(blobinfo[blobnr-1].nof_corners < 4){
                    blobinfo[blobnr-1].corners[blobinfo[blobnr-1].nof_corners] = label;
                }
                blobinfo[blobnr-1].nof_corners++;
                label++;
            }
        }
    }

    return label;
}

double Det2( double a, double b, double c, double d )
{
    return ( a * d - b * c );
}

uint8_t vCheckHamming(uint8_t hamming){
    if(hamming == 0x10){
        return 1;
    } else if(hamming == 0x17){
        return 1;
    } else if(hamming == 0x09){
        return 1;
    } else if(hamming == 0x0E){
        return 1;
    } else {
        return 0;
    }
}

int32_t vGetMarkerId(image_t *img){

    register uint32_t k, xM, yM, x, y, pixVal = 0;
    uint8_t hammingCheck;
    uint8_t hamTry = 0;
    uint8_t marker[7][7]; //container marker

    for(xM = 0; xM < 7; xM++){
        for(yM = 0; yM < 7; yM++){

            for(x = 0; x < 10; x++){
                for(y = 0; y < 10; y++){
                    pixVal += img->data[yM*10+y][xM*10+x];
                }
            }

            marker[yM][xM] = pixVal / 100;

            pixVal = 0;

            img->data[yM+80][xM+80] = marker[yM][xM];
        }
    }

    int av = vThresholdIsoDataInternal(&marker[0][0],7,7);

    for(xM = 0; xM < 7; xM++){
        for(yM = 0; yM < 7; yM++){
            marker[yM][xM] = (marker[yM][xM] > av);
        }
    }

    for(k = 1; k < 6; k++){
        hammingCheck = (marker[k][1] << 4) | (marker[k][2] << 3) | (marker[k][3] << 2) | (marker[k][4] << 1) | marker[k][5];
        if(vCheckHamming(hammingCheck) == 0){
            hamTry = 1;
            break;
        }
    }

    if(hamTry > 0){
        for(k = 1; k < 6; k++){
            hammingCheck = (marker[k][1]) | (marker[k][2] << 1) | (marker[k][3] << 2) | (marker[k][4] << 3) | (marker[k][5] << 4);
            if(vCheckHamming(hammingCheck) == 0){
                hamTry = 2;
                break;
            }
        }
    }

    if(hamTry > 1){
        for(k = 1; k < 6; k++){
            hammingCheck = (marker[1][k] << 4) | (marker[2][k] << 3) | (marker[3][k] << 2) | (marker[4][k] << 1) | marker[5][k];
            if(vCheckHamming(hammingCheck) == 0){
                hamTry = 3;
                break;
            }
        }
    }

    if(hamTry > 2){
        for(k = 1; k < 6; k++){
            hammingCheck = (marker[1][k]) | (marker[2][k] << 1) | (marker[3][k] << 2) | (marker[4][k] << 3) | (marker[5][k] << 4);
            if(vCheckHamming(hammingCheck) == 0){
                return -1;
                break;
            }
        }
    }

    if(hamTry == 0){
        return (marker[1][2] << 9 | marker[1][4] << 8 | marker[2][2] << 7 | marker[2][4] << 6
                | marker[3][2] << 5 | marker[3][4] << 4 | marker[4][2] << 3 | marker[4][4] << 2
                | marker[5][2] << 1 | marker[5][4]);
    } else if(hamTry == 1){
        return (marker[1][2] | marker[1][4] << 1 | marker[2][2] << 2 | marker[2][4] << 3
                | marker[3][2] << 4 | marker[3][4] << 5 | marker[4][2] << 6 | marker[4][4] << 7
                | marker[5][2] << 8 | marker[5][4] << 9);
    } else if(hamTry == 2){
        return (marker[2][5] << 9 | marker[4][5] << 8 | marker[2][4] << 7 | marker[4][4] << 6
                | marker[2][3] << 5 | marker[4][3] << 4 | marker[2][2] << 3 | marker[4][2] << 2
                | marker[2][1] << 1 | marker[4][1]);
    } else if(hamTry == 3){
        return (marker[4][1] << 9 | marker[2][1] << 8 | marker[4][2] << 7 | marker[2][2] << 6
                | marker[4][3] << 5 | marker[2][3] << 4 | marker[4][4] << 3 | marker[2][4] << 2
                | marker[4][5] << 1 | marker[2][5]);

    }
}

void vPerspectiveTransform(image_t *src,image_t *dst,
                           uint8_t dstRectSize, xy quad[4]){

    //Set the dest quare matrix based on size
    double dstRectMatrix[3][3] = { {dstRectSize,0,0},
                                   {0,dstRectSize,0},
                                   {0,0,Det2( dstRectSize, 0, 0, dstRectSize)}};

    //Set the 4 quad points in the srcQuad matrix
    double srcQuadMatrix[3][3];
    double dx1, dx2, dy1, dy2, del, px, py;

    px = quad[0].x - quad[1].x + quad[2].x - quad[3].x;
    py = quad[0].y - quad[1].y + quad[2].y - quad[3].y;
    dx1 = quad[1].x - quad[2].x;
    dx2 = quad[3].x - quad[2].x;
    dy1 = quad[1].y - quad[2].y;
    dy2 = quad[3].y - quad[2].y;
    del = Det2( dx1, dx2, dy1, dy2 );

    srcQuadMatrix[2][0] = Det2( px, dx2, py, dy2 ) / del;
    srcQuadMatrix[2][1] = Det2( dx1, px, dy1, py ) / del;
    srcQuadMatrix[2][2] = 1.0;

    srcQuadMatrix[0][0] = (double)quad[1].x - (double)quad[0].x + srcQuadMatrix[2][0] * (double)quad[1].x;
    srcQuadMatrix[0][1] = (double)quad[3].x - (double)quad[0].x + srcQuadMatrix[2][1] * (double)quad[3].x;
    srcQuadMatrix[0][2] = (double)quad[0].x;

    srcQuadMatrix[1][0] = (double)quad[1].y - (double)quad[0].y + srcQuadMatrix[2][0] * (double)quad[1].y;
    srcQuadMatrix[1][1] = (double)quad[3].y - (double)quad[0].y + srcQuadMatrix[2][1] * (double)quad[3].y;
    srcQuadMatrix[1][2] = (double)quad[0].y;

    //Mutiply src with dst matrix for the transformation
    double transformMatrix[3][3];
    transformMatrix[0][0] = srcQuadMatrix[0][0] * dstRectMatrix[0][0] + srcQuadMatrix[0][1] * dstRectMatrix[1][0] + srcQuadMatrix[0][2] * dstRectMatrix[2][0];
    transformMatrix[0][1] = srcQuadMatrix[0][0] * dstRectMatrix[0][1] + srcQuadMatrix[0][1] * dstRectMatrix[1][1] + srcQuadMatrix[0][2] * dstRectMatrix[2][1];
    transformMatrix[0][2] = srcQuadMatrix[0][0] * dstRectMatrix[0][2] + srcQuadMatrix[0][1] * dstRectMatrix[1][2] + srcQuadMatrix[0][2] * dstRectMatrix[2][2];
    transformMatrix[1][0] = srcQuadMatrix[1][0] * dstRectMatrix[0][0] + srcQuadMatrix[1][1] * dstRectMatrix[1][0] + srcQuadMatrix[1][2] * dstRectMatrix[2][0];
    transformMatrix[1][1] = srcQuadMatrix[1][0] * dstRectMatrix[0][1] + srcQuadMatrix[1][1] * dstRectMatrix[1][1] + srcQuadMatrix[1][2] * dstRectMatrix[2][1];
    transformMatrix[1][2] = srcQuadMatrix[1][0] * dstRectMatrix[0][2] + srcQuadMatrix[1][1] * dstRectMatrix[1][2] + srcQuadMatrix[1][2] * dstRectMatrix[2][2];
    transformMatrix[2][0] = srcQuadMatrix[2][0] * dstRectMatrix[0][0] + srcQuadMatrix[2][1] * dstRectMatrix[1][0] + srcQuadMatrix[2][2] * dstRectMatrix[2][0];
    transformMatrix[2][1] = srcQuadMatrix[2][0] * dstRectMatrix[0][1] + srcQuadMatrix[2][1] * dstRectMatrix[1][1] + srcQuadMatrix[2][2] * dstRectMatrix[2][1];
    transformMatrix[2][2] = srcQuadMatrix[2][0] * dstRectMatrix[0][2] + srcQuadMatrix[2][1] * dstRectMatrix[1][2] + srcQuadMatrix[2][2] * dstRectMatrix[2][2];

    register uint32_t x, y;
    double factor,srcX,srcY;

    for(y = 0; y < dstRectSize; y++){
        for(x = 0; x < dstRectSize; x++){
            factor = transformMatrix[2][0] * x + transformMatrix[2][1] * y + transformMatrix[2][2];
            srcX = ( transformMatrix[0][0] * x + transformMatrix[0][1] * y + transformMatrix[0][2] ) / factor;
            srcY = ( transformMatrix[1][0] * x + transformMatrix[1][1] * y + transformMatrix[1][2] ) / factor;

            if ( ( srcX >= 0 ) && ( srcY >= 0 ) && ( srcX < src->width ) && ( srcY < src->height ) )
            {
                dst->data[y][x] = src->data[(int)srcY][(int)srcX];
            }

        }
    }
}

void vRotateDegrees(image_t *src, image_t *dst, double rotation, int x_move, int y_move){
    double affine[2][2], beta[2];
    double det, i_affine[2][2], i_beta[2];
    double x_new, y_new, x_frac, y_frac;
    double gray_new;
    int x, y, m, n;

    int img_width = src->width;
    int img_height = src->height;

    /* forward affine transformation */
    affine[0][0] = cos(rotation*PI/180.0);
    affine[0][1] = sin(0-rotation*PI/180.0);
    affine[1][0] = sin(rotation*PI/180.0);
    affine[1][1] = cos(0-rotation*PI/180.0);
    beta[0] = x_move;
    beta[1] = y_move;

    /* determination of inverse affine transformation */
    det = affine[0][0]*affine[1][1] - affine[0][1]*affine[1][0];
    if (det == 0.0) {
        i_affine[0][0] = 1.0;
        i_affine[0][1] = 0.0;
        i_affine[1][0] = 0.0;
        i_affine[1][1] = 1.0;
        i_beta[0] = -beta[0];
        i_beta[1] = -beta[1];
    } else {
        i_affine[0][0] =  affine[1][1]/det;
        i_affine[0][1] = -affine[0][1]/det;
        i_affine[1][0] = -affine[1][0]/det;
        i_affine[1][1] =  affine[0][0]/det;
        i_beta[0] = -i_affine[0][0]*beta[0]-i_affine[0][1]*beta[1];
        i_beta[1] = -i_affine[1][0]*beta[0]-i_affine[1][1]*beta[1];
    }

    /* output image generation by inverse affine transformation
         and bilinear transformation  */
    for (y = 0; y < img_height; y++) {
        for (x = 0; x < img_width; x++) {
            x_new = i_beta[0] + i_affine[0][0]*(x-img_width/2.0)
                    + i_affine[0][1]*(y-img_height/2.0) + img_width/2.0;
            y_new = i_beta[1] + i_affine[1][0]*(x-img_width/2.0)
                    + i_affine[1][1]*(y-img_height/2.0) + img_height/2.0;
            m = (int)floor(x_new);
            n = (int)floor(y_new);
            x_frac = x_new - m;
            y_frac = y_new - n;

            if (m >= 0 && m+1 < img_width && n >= 0 && n+1 < img_height) {
                gray_new = (1.0 - y_frac)*((1.0 - x_frac)*src->data[n][m] +
                                           x_frac *src->data[n][m+1]) +
                        y_frac *((1.0 - x_frac)*src->data[n+1][m] +
                        x_frac *src->data[n+1][m+1]);
                dst->data[y][x] = (unsigned char)gray_new;
            } else if (m+1 == img_width && n >= 0 && n < img_height
                       || n+1 == img_height && m >= 0 && m < img_width) {
                dst->data[y][x] = src->data[n][m];
            } else {
                dst->data[y][x] = 255;
            }
        }
    }
}

void vWarp(image_t *src, image_t *dst, uint8_t sx, uint8_t sy, uint8_t ex, uint8_t ey,
           int *Wx, int *Wy){

    //vErase(dst);
    vSetRangeToValue(dst,dst,0,255,0);

    float a,b,c,d,e,f,i,j,x,y,destX,destY;
    int dy, x2;

    //Set up warp coeff
    a = (float)(-(*Wx) +  *(Wx+1))/(ey-sy);
    b = (float)(-(*Wx) +  *(Wx+3))/(ex-sx);
    c = (float)((*Wx) - *(Wx+1) + *(Wx+2) -
                *(Wx+3))/((ey-sy)*(ex-sx));

    d = (float)(*Wx);
    e = (float)(-(*Wy) + *(Wy+1))/(ex-sx);
    f = (float)(-(*Wy) + *(Wy+3))/(ey-sy);
    i = (float)((*Wy) -  *(Wy+1) + *(Wy+2) -
                *(Wy+3))/((ey-sy)*(ex-sx));

    j = (float)(*Wy);

    //Warp
    for(y=sy; y<ey; y+=0.5){
        dy = (int)y+0.5;
        for(x=sx; x<ex; x+=0.5){
            destX = a*x + b*y + c*x*y + d;
            destY = e*x + f*y + i*x*y + j;
            /* *(&dst->data[0][0] + (dst->width * (int)(destY+0.5)) + (int)destX ) =
                    *(&src->data[0][0] + (long)(y*src->width)+ (long)(x+.05));*/
            /*if(destX > 0 && destY > 0){
                dst->data[(int)destY][(int)destX] = src->data[(int)y][(int)x];
            }*/
            /*
            *(Out->Data + (Out->cols * (int)(destY+0.5)) + (int)destX)
                =
            *(Img->Data + (long)(y*Img->cols)+(long)(x+.05));
            */

            // qDebug() << "DestX:" << (int)destX << "DestY:" << (int)destY;
            // qDebug() << "Value" << *(&src->data[0][0] + (long)(y*src->height)+(long)(x+.05));

            /*  *(&dst->data[0][0] + (dst->height * (int)(destY+0.5)) + (int)destX) =
            *(&src->data[0][0] + (long)(y*src->height)+(long)(x+0.05));*/

            x2 = (int)(x+0.05);

            if(destX >= 0 && destX <= dst->width && destY >= 0 && destY <= dst->height
                    && x2 >= 0 && x2 <= dst->width && y >= 0 && y <= src->height){
                dst->data[(int)destY][(int)destX] = src->data[(int)y][x2];
            }
            // *(&src->data[0][0] + (long)(y*src->height)+(long)(x+.05));
        }
    }

}

void vClearNeighBours(image_t *src, image_t *dst, int value){
    if(src != dst)
        vCopy(src,dst);

    register uint32_t x,y;


    for(y=1; y < dst->height; y++){
        for(x=1; x < dst->width; x++){
            if(dst->data[y][x] == value){
                if(iNeighbourCount(dst,x,y,value,EIGHT) == 0){
                    dst->data[y][x] = 0;
                }
            }
        }
    }

}

xy* fast9_detect(const uint8_t* im, int xsize, int ysize, int stride, int b, int* ret_num_corners)
{
    int num_corners=0;
    xy* ret_corners;
    int rsize=512;
    int pixel[16];
    int x, y;

    ret_corners = (xy*)malloc(sizeof(xy)*rsize);
    make_offsets(pixel, stride);

    for(y=3; y < ysize - 3; y++)
        for(x=3; x < xsize - 3; x++)
        {
            const byte* p = im + y*stride + x;

            int cb = *p + b;
            int c_b= *p - b;
            if(p[pixel[0]] > cb)
                if(p[pixel[1]] > cb)
                    if(p[pixel[2]] > cb)
                        if(p[pixel[3]] > cb)
                            if(p[pixel[4]] > cb)
                                if(p[pixel[5]] > cb)
                                    if(p[pixel[6]] > cb)
                                        if(p[pixel[7]] > cb)
                                            if(p[pixel[8]] > cb)
                                            {}
                                            else
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    continue;
                                        else if(p[pixel[7]] < c_b)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    continue;
                                            else if(p[pixel[14]] < c_b)
                                                if(p[pixel[8]] < c_b)
                                                    if(p[pixel[9]] < c_b)
                                                        if(p[pixel[10]] < c_b)
                                                            if(p[pixel[11]] < c_b)
                                                                if(p[pixel[12]] < c_b)
                                                                    if(p[pixel[13]] < c_b)
                                                                        if(p[pixel[15]] < c_b)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else if(p[pixel[6]] < c_b)
                                        if(p[pixel[15]] > cb)
                                            if(p[pixel[13]] > cb)
                                                if(p[pixel[14]] > cb)
                                                {}
                                                else
                                                    continue;
                                            else if(p[pixel[13]] < c_b)
                                                if(p[pixel[7]] < c_b)
                                                    if(p[pixel[8]] < c_b)
                                                        if(p[pixel[9]] < c_b)
                                                            if(p[pixel[10]] < c_b)
                                                                if(p[pixel[11]] < c_b)
                                                                    if(p[pixel[12]] < c_b)
                                                                        if(p[pixel[14]] < c_b)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            if(p[pixel[7]] < c_b)
                                                if(p[pixel[8]] < c_b)
                                                    if(p[pixel[9]] < c_b)
                                                        if(p[pixel[10]] < c_b)
                                                            if(p[pixel[11]] < c_b)
                                                                if(p[pixel[12]] < c_b)
                                                                    if(p[pixel[13]] < c_b)
                                                                        if(p[pixel[14]] < c_b)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[13]] > cb)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if(p[pixel[13]] < c_b)
                                            if(p[pixel[7]] < c_b)
                                                if(p[pixel[8]] < c_b)
                                                    if(p[pixel[9]] < c_b)
                                                        if(p[pixel[10]] < c_b)
                                                            if(p[pixel[11]] < c_b)
                                                                if(p[pixel[12]] < c_b)
                                                                    if(p[pixel[14]] < c_b)
                                                                        if(p[pixel[15]] < c_b)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else if(p[pixel[5]] < c_b)
                                    if(p[pixel[14]] > cb)
                                        if(p[pixel[12]] > cb)
                                            if(p[pixel[13]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                    if(p[pixel[10]] > cb)
                                                                        if(p[pixel[11]] > cb)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                continue;
                                        else if(p[pixel[12]] < c_b)
                                            if(p[pixel[6]] < c_b)
                                                if(p[pixel[7]] < c_b)
                                                    if(p[pixel[8]] < c_b)
                                                        if(p[pixel[9]] < c_b)
                                                            if(p[pixel[10]] < c_b)
                                                                if(p[pixel[11]] < c_b)
                                                                    if(p[pixel[13]] < c_b)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if(p[pixel[14]] < c_b)
                                        if(p[pixel[7]] < c_b)
                                            if(p[pixel[8]] < c_b)
                                                if(p[pixel[9]] < c_b)
                                                    if(p[pixel[10]] < c_b)
                                                        if(p[pixel[11]] < c_b)
                                                            if(p[pixel[12]] < c_b)
                                                                if(p[pixel[13]] < c_b)
                                                                    if(p[pixel[6]] < c_b)
                                                                    {}
                                                                    else
                                                                        if(p[pixel[15]] < c_b)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        if(p[pixel[6]] < c_b)
                                            if(p[pixel[7]] < c_b)
                                                if(p[pixel[8]] < c_b)
                                                    if(p[pixel[9]] < c_b)
                                                        if(p[pixel[10]] < c_b)
                                                            if(p[pixel[11]] < c_b)
                                                                if(p[pixel[12]] < c_b)
                                                                    if(p[pixel[13]] < c_b)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(p[pixel[12]] > cb)
                                        if(p[pixel[13]] > cb)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                    if(p[pixel[10]] > cb)
                                                                        if(p[pixel[11]] > cb)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if(p[pixel[12]] < c_b)
                                        if(p[pixel[7]] < c_b)
                                            if(p[pixel[8]] < c_b)
                                                if(p[pixel[9]] < c_b)
                                                    if(p[pixel[10]] < c_b)
                                                        if(p[pixel[11]] < c_b)
                                                            if(p[pixel[13]] < c_b)
                                                                if(p[pixel[14]] < c_b)
                                                                    if(p[pixel[6]] < c_b)
                                                                    {}
                                                                    else
                                                                        if(p[pixel[15]] < c_b)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else if(p[pixel[4]] < c_b)
                                if(p[pixel[13]] > cb)
                                    if(p[pixel[11]] > cb)
                                        if(p[pixel[12]] > cb)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                    if(p[pixel[10]] > cb)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                    if(p[pixel[10]] > cb)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else if(p[pixel[11]] < c_b)
                                        if(p[pixel[5]] < c_b)
                                            if(p[pixel[6]] < c_b)
                                                if(p[pixel[7]] < c_b)
                                                    if(p[pixel[8]] < c_b)
                                                        if(p[pixel[9]] < c_b)
                                                            if(p[pixel[10]] < c_b)
                                                                if(p[pixel[12]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(p[pixel[13]] < c_b)
                                    if(p[pixel[7]] < c_b)
                                        if(p[pixel[8]] < c_b)
                                            if(p[pixel[9]] < c_b)
                                                if(p[pixel[10]] < c_b)
                                                    if(p[pixel[11]] < c_b)
                                                        if(p[pixel[12]] < c_b)
                                                            if(p[pixel[6]] < c_b)
                                                                if(p[pixel[5]] < c_b)
                                                                {}
                                                                else
                                                                    if(p[pixel[14]] < c_b)
                                                                    {}
                                                                    else
                                                                        continue;
                                                            else
                                                                if(p[pixel[14]] < c_b)
                                                                    if(p[pixel[15]] < c_b)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    if(p[pixel[5]] < c_b)
                                        if(p[pixel[6]] < c_b)
                                            if(p[pixel[7]] < c_b)
                                                if(p[pixel[8]] < c_b)
                                                    if(p[pixel[9]] < c_b)
                                                        if(p[pixel[10]] < c_b)
                                                            if(p[pixel[11]] < c_b)
                                                                if(p[pixel[12]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                if(p[pixel[11]] > cb)
                                    if(p[pixel[12]] > cb)
                                        if(p[pixel[13]] > cb)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                    if(p[pixel[10]] > cb)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                    if(p[pixel[10]] > cb)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(p[pixel[11]] < c_b)
                                    if(p[pixel[7]] < c_b)
                                        if(p[pixel[8]] < c_b)
                                            if(p[pixel[9]] < c_b)
                                                if(p[pixel[10]] < c_b)
                                                    if(p[pixel[12]] < c_b)
                                                        if(p[pixel[13]] < c_b)
                                                            if(p[pixel[6]] < c_b)
                                                                if(p[pixel[5]] < c_b)
                                                                {}
                                                                else
                                                                    if(p[pixel[14]] < c_b)
                                                                    {}
                                                                    else
                                                                        continue;
                                                            else
                                                                if(p[pixel[14]] < c_b)
                                                                    if(p[pixel[15]] < c_b)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else if(p[pixel[3]] < c_b)
                            if(p[pixel[10]] > cb)
                                if(p[pixel[11]] > cb)
                                    if(p[pixel[12]] > cb)
                                        if(p[pixel[13]] > cb)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if(p[pixel[10]] < c_b)
                                if(p[pixel[7]] < c_b)
                                    if(p[pixel[8]] < c_b)
                                        if(p[pixel[9]] < c_b)
                                            if(p[pixel[11]] < c_b)
                                                if(p[pixel[6]] < c_b)
                                                    if(p[pixel[5]] < c_b)
                                                        if(p[pixel[4]] < c_b)
                                                        {}
                                                        else
                                                            if(p[pixel[12]] < c_b)
                                                                if(p[pixel[13]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                    else
                                                        if(p[pixel[12]] < c_b)
                                                            if(p[pixel[13]] < c_b)
                                                                if(p[pixel[14]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else
                                                    if(p[pixel[12]] < c_b)
                                                        if(p[pixel[13]] < c_b)
                                                            if(p[pixel[14]] < c_b)
                                                                if(p[pixel[15]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            if(p[pixel[10]] > cb)
                                if(p[pixel[11]] > cb)
                                    if(p[pixel[12]] > cb)
                                        if(p[pixel[13]] > cb)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                                if(p[pixel[9]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if(p[pixel[10]] < c_b)
                                if(p[pixel[7]] < c_b)
                                    if(p[pixel[8]] < c_b)
                                        if(p[pixel[9]] < c_b)
                                            if(p[pixel[11]] < c_b)
                                                if(p[pixel[12]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[5]] < c_b)
                                                            if(p[pixel[4]] < c_b)
                                                            {}
                                                            else
                                                                if(p[pixel[13]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                        else
                                                            if(p[pixel[13]] < c_b)
                                                                if(p[pixel[14]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                    else
                                                        if(p[pixel[13]] < c_b)
                                                            if(p[pixel[14]] < c_b)
                                                                if(p[pixel[15]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                    else if(p[pixel[2]] < c_b)
                        if(p[pixel[9]] > cb)
                            if(p[pixel[10]] > cb)
                                if(p[pixel[11]] > cb)
                                    if(p[pixel[12]] > cb)
                                        if(p[pixel[13]] > cb)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[3]] > cb)
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else
                                continue;
                        else if(p[pixel[9]] < c_b)
                            if(p[pixel[7]] < c_b)
                                if(p[pixel[8]] < c_b)
                                    if(p[pixel[10]] < c_b)
                                        if(p[pixel[6]] < c_b)
                                            if(p[pixel[5]] < c_b)
                                                if(p[pixel[4]] < c_b)
                                                    if(p[pixel[3]] < c_b)
                                                    {}
                                                    else
                                                        if(p[pixel[11]] < c_b)
                                                            if(p[pixel[12]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else
                                                    if(p[pixel[11]] < c_b)
                                                        if(p[pixel[12]] < c_b)
                                                            if(p[pixel[13]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[11]] < c_b)
                                                    if(p[pixel[12]] < c_b)
                                                        if(p[pixel[13]] < c_b)
                                                            if(p[pixel[14]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[11]] < c_b)
                                                if(p[pixel[12]] < c_b)
                                                    if(p[pixel[13]] < c_b)
                                                        if(p[pixel[14]] < c_b)
                                                            if(p[pixel[15]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        if(p[pixel[9]] > cb)
                            if(p[pixel[10]] > cb)
                                if(p[pixel[11]] > cb)
                                    if(p[pixel[12]] > cb)
                                        if(p[pixel[13]] > cb)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[3]] > cb)
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                            if(p[pixel[8]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else
                                continue;
                        else if(p[pixel[9]] < c_b)
                            if(p[pixel[7]] < c_b)
                                if(p[pixel[8]] < c_b)
                                    if(p[pixel[10]] < c_b)
                                        if(p[pixel[11]] < c_b)
                                            if(p[pixel[6]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[4]] < c_b)
                                                        if(p[pixel[3]] < c_b)
                                                        {}
                                                        else
                                                            if(p[pixel[12]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                    else
                                                        if(p[pixel[12]] < c_b)
                                                            if(p[pixel[13]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else
                                                    if(p[pixel[12]] < c_b)
                                                        if(p[pixel[13]] < c_b)
                                                            if(p[pixel[14]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[12]] < c_b)
                                                    if(p[pixel[13]] < c_b)
                                                        if(p[pixel[14]] < c_b)
                                                            if(p[pixel[15]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                else if(p[pixel[1]] < c_b)
                    if(p[pixel[8]] > cb)
                        if(p[pixel[9]] > cb)
                            if(p[pixel[10]] > cb)
                                if(p[pixel[11]] > cb)
                                    if(p[pixel[12]] > cb)
                                        if(p[pixel[13]] > cb)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[3]] > cb)
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(p[pixel[2]] > cb)
                                        if(p[pixel[3]] > cb)
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                continue;
                        else
                            continue;
                    else if(p[pixel[8]] < c_b)
                        if(p[pixel[7]] < c_b)
                            if(p[pixel[9]] < c_b)
                                if(p[pixel[6]] < c_b)
                                    if(p[pixel[5]] < c_b)
                                        if(p[pixel[4]] < c_b)
                                            if(p[pixel[3]] < c_b)
                                                if(p[pixel[2]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[10]] < c_b)
                                                        if(p[pixel[11]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[10]] < c_b)
                                                    if(p[pixel[11]] < c_b)
                                                        if(p[pixel[12]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[10]] < c_b)
                                                if(p[pixel[11]] < c_b)
                                                    if(p[pixel[12]] < c_b)
                                                        if(p[pixel[13]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[10]] < c_b)
                                            if(p[pixel[11]] < c_b)
                                                if(p[pixel[12]] < c_b)
                                                    if(p[pixel[13]] < c_b)
                                                        if(p[pixel[14]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(p[pixel[10]] < c_b)
                                        if(p[pixel[11]] < c_b)
                                            if(p[pixel[12]] < c_b)
                                                if(p[pixel[13]] < c_b)
                                                    if(p[pixel[14]] < c_b)
                                                        if(p[pixel[15]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else
                    if(p[pixel[8]] > cb)
                        if(p[pixel[9]] > cb)
                            if(p[pixel[10]] > cb)
                                if(p[pixel[11]] > cb)
                                    if(p[pixel[12]] > cb)
                                        if(p[pixel[13]] > cb)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[15]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[3]] > cb)
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(p[pixel[2]] > cb)
                                        if(p[pixel[3]] > cb)
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[7]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                continue;
                        else
                            continue;
                    else if(p[pixel[8]] < c_b)
                        if(p[pixel[7]] < c_b)
                            if(p[pixel[9]] < c_b)
                                if(p[pixel[10]] < c_b)
                                    if(p[pixel[6]] < c_b)
                                        if(p[pixel[5]] < c_b)
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[3]] < c_b)
                                                    if(p[pixel[2]] < c_b)
                                                    {}
                                                    else
                                                        if(p[pixel[11]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                else
                                                    if(p[pixel[11]] < c_b)
                                                        if(p[pixel[12]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[11]] < c_b)
                                                    if(p[pixel[12]] < c_b)
                                                        if(p[pixel[13]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[11]] < c_b)
                                                if(p[pixel[12]] < c_b)
                                                    if(p[pixel[13]] < c_b)
                                                        if(p[pixel[14]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[11]] < c_b)
                                            if(p[pixel[12]] < c_b)
                                                if(p[pixel[13]] < c_b)
                                                    if(p[pixel[14]] < c_b)
                                                        if(p[pixel[15]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
            else if(p[pixel[0]] < c_b)
                if(p[pixel[1]] > cb)
                    if(p[pixel[8]] > cb)
                        if(p[pixel[7]] > cb)
                            if(p[pixel[9]] > cb)
                                if(p[pixel[6]] > cb)
                                    if(p[pixel[5]] > cb)
                                        if(p[pixel[4]] > cb)
                                            if(p[pixel[3]] > cb)
                                                if(p[pixel[2]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[10]] > cb)
                                                        if(p[pixel[11]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[10]] > cb)
                                                    if(p[pixel[11]] > cb)
                                                        if(p[pixel[12]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[10]] > cb)
                                                if(p[pixel[11]] > cb)
                                                    if(p[pixel[12]] > cb)
                                                        if(p[pixel[13]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[10]] > cb)
                                            if(p[pixel[11]] > cb)
                                                if(p[pixel[12]] > cb)
                                                    if(p[pixel[13]] > cb)
                                                        if(p[pixel[14]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(p[pixel[10]] > cb)
                                        if(p[pixel[11]] > cb)
                                            if(p[pixel[12]] > cb)
                                                if(p[pixel[13]] > cb)
                                                    if(p[pixel[14]] > cb)
                                                        if(p[pixel[15]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                continue;
                        else
                            continue;
                    else if(p[pixel[8]] < c_b)
                        if(p[pixel[9]] < c_b)
                            if(p[pixel[10]] < c_b)
                                if(p[pixel[11]] < c_b)
                                    if(p[pixel[12]] < c_b)
                                        if(p[pixel[13]] < c_b)
                                            if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[3]] < c_b)
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(p[pixel[2]] < c_b)
                                        if(p[pixel[3]] < c_b)
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else if(p[pixel[1]] < c_b)
                    if(p[pixel[2]] > cb)
                        if(p[pixel[9]] > cb)
                            if(p[pixel[7]] > cb)
                                if(p[pixel[8]] > cb)
                                    if(p[pixel[10]] > cb)
                                        if(p[pixel[6]] > cb)
                                            if(p[pixel[5]] > cb)
                                                if(p[pixel[4]] > cb)
                                                    if(p[pixel[3]] > cb)
                                                    {}
                                                    else
                                                        if(p[pixel[11]] > cb)
                                                            if(p[pixel[12]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else
                                                    if(p[pixel[11]] > cb)
                                                        if(p[pixel[12]] > cb)
                                                            if(p[pixel[13]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[11]] > cb)
                                                    if(p[pixel[12]] > cb)
                                                        if(p[pixel[13]] > cb)
                                                            if(p[pixel[14]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[11]] > cb)
                                                if(p[pixel[12]] > cb)
                                                    if(p[pixel[13]] > cb)
                                                        if(p[pixel[14]] > cb)
                                                            if(p[pixel[15]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if(p[pixel[9]] < c_b)
                            if(p[pixel[10]] < c_b)
                                if(p[pixel[11]] < c_b)
                                    if(p[pixel[12]] < c_b)
                                        if(p[pixel[13]] < c_b)
                                            if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[3]] < c_b)
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else if(p[pixel[2]] < c_b)
                        if(p[pixel[3]] > cb)
                            if(p[pixel[10]] > cb)
                                if(p[pixel[7]] > cb)
                                    if(p[pixel[8]] > cb)
                                        if(p[pixel[9]] > cb)
                                            if(p[pixel[11]] > cb)
                                                if(p[pixel[6]] > cb)
                                                    if(p[pixel[5]] > cb)
                                                        if(p[pixel[4]] > cb)
                                                        {}
                                                        else
                                                            if(p[pixel[12]] > cb)
                                                                if(p[pixel[13]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                    else
                                                        if(p[pixel[12]] > cb)
                                                            if(p[pixel[13]] > cb)
                                                                if(p[pixel[14]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else
                                                    if(p[pixel[12]] > cb)
                                                        if(p[pixel[13]] > cb)
                                                            if(p[pixel[14]] > cb)
                                                                if(p[pixel[15]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if(p[pixel[10]] < c_b)
                                if(p[pixel[11]] < c_b)
                                    if(p[pixel[12]] < c_b)
                                        if(p[pixel[13]] < c_b)
                                            if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if(p[pixel[3]] < c_b)
                            if(p[pixel[4]] > cb)
                                if(p[pixel[13]] > cb)
                                    if(p[pixel[7]] > cb)
                                        if(p[pixel[8]] > cb)
                                            if(p[pixel[9]] > cb)
                                                if(p[pixel[10]] > cb)
                                                    if(p[pixel[11]] > cb)
                                                        if(p[pixel[12]] > cb)
                                                            if(p[pixel[6]] > cb)
                                                                if(p[pixel[5]] > cb)
                                                                {}
                                                                else
                                                                    if(p[pixel[14]] > cb)
                                                                    {}
                                                                    else
                                                                        continue;
                                                            else
                                                                if(p[pixel[14]] > cb)
                                                                    if(p[pixel[15]] > cb)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(p[pixel[13]] < c_b)
                                    if(p[pixel[11]] > cb)
                                        if(p[pixel[5]] > cb)
                                            if(p[pixel[6]] > cb)
                                                if(p[pixel[7]] > cb)
                                                    if(p[pixel[8]] > cb)
                                                        if(p[pixel[9]] > cb)
                                                            if(p[pixel[10]] > cb)
                                                                if(p[pixel[12]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if(p[pixel[11]] < c_b)
                                        if(p[pixel[12]] < c_b)
                                            if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                    if(p[pixel[10]] < c_b)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                    if(p[pixel[10]] < c_b)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    if(p[pixel[5]] > cb)
                                        if(p[pixel[6]] > cb)
                                            if(p[pixel[7]] > cb)
                                                if(p[pixel[8]] > cb)
                                                    if(p[pixel[9]] > cb)
                                                        if(p[pixel[10]] > cb)
                                                            if(p[pixel[11]] > cb)
                                                                if(p[pixel[12]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else if(p[pixel[4]] < c_b)
                                if(p[pixel[5]] > cb)
                                    if(p[pixel[14]] > cb)
                                        if(p[pixel[7]] > cb)
                                            if(p[pixel[8]] > cb)
                                                if(p[pixel[9]] > cb)
                                                    if(p[pixel[10]] > cb)
                                                        if(p[pixel[11]] > cb)
                                                            if(p[pixel[12]] > cb)
                                                                if(p[pixel[13]] > cb)
                                                                    if(p[pixel[6]] > cb)
                                                                    {}
                                                                    else
                                                                        if(p[pixel[15]] > cb)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if(p[pixel[14]] < c_b)
                                        if(p[pixel[12]] > cb)
                                            if(p[pixel[6]] > cb)
                                                if(p[pixel[7]] > cb)
                                                    if(p[pixel[8]] > cb)
                                                        if(p[pixel[9]] > cb)
                                                            if(p[pixel[10]] > cb)
                                                                if(p[pixel[11]] > cb)
                                                                    if(p[pixel[13]] > cb)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if(p[pixel[12]] < c_b)
                                            if(p[pixel[13]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                    if(p[pixel[10]] < c_b)
                                                                        if(p[pixel[11]] < c_b)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        if(p[pixel[6]] > cb)
                                            if(p[pixel[7]] > cb)
                                                if(p[pixel[8]] > cb)
                                                    if(p[pixel[9]] > cb)
                                                        if(p[pixel[10]] > cb)
                                                            if(p[pixel[11]] > cb)
                                                                if(p[pixel[12]] > cb)
                                                                    if(p[pixel[13]] > cb)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else if(p[pixel[5]] < c_b)
                                    if(p[pixel[6]] > cb)
                                        if(p[pixel[15]] < c_b)
                                            if(p[pixel[13]] > cb)
                                                if(p[pixel[7]] > cb)
                                                    if(p[pixel[8]] > cb)
                                                        if(p[pixel[9]] > cb)
                                                            if(p[pixel[10]] > cb)
                                                                if(p[pixel[11]] > cb)
                                                                    if(p[pixel[12]] > cb)
                                                                        if(p[pixel[14]] > cb)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if(p[pixel[13]] < c_b)
                                                if(p[pixel[14]] < c_b)
                                                {}
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            if(p[pixel[7]] > cb)
                                                if(p[pixel[8]] > cb)
                                                    if(p[pixel[9]] > cb)
                                                        if(p[pixel[10]] > cb)
                                                            if(p[pixel[11]] > cb)
                                                                if(p[pixel[12]] > cb)
                                                                    if(p[pixel[13]] > cb)
                                                                        if(p[pixel[14]] > cb)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else if(p[pixel[6]] < c_b)
                                        if(p[pixel[7]] > cb)
                                            if(p[pixel[14]] > cb)
                                                if(p[pixel[8]] > cb)
                                                    if(p[pixel[9]] > cb)
                                                        if(p[pixel[10]] > cb)
                                                            if(p[pixel[11]] > cb)
                                                                if(p[pixel[12]] > cb)
                                                                    if(p[pixel[13]] > cb)
                                                                        if(p[pixel[15]] > cb)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if(p[pixel[7]] < c_b)
                                            if(p[pixel[8]] < c_b)
                                            {}
                                            else
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[13]] > cb)
                                            if(p[pixel[7]] > cb)
                                                if(p[pixel[8]] > cb)
                                                    if(p[pixel[9]] > cb)
                                                        if(p[pixel[10]] > cb)
                                                            if(p[pixel[11]] > cb)
                                                                if(p[pixel[12]] > cb)
                                                                    if(p[pixel[14]] > cb)
                                                                        if(p[pixel[15]] > cb)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if(p[pixel[13]] < c_b)
                                            if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(p[pixel[12]] > cb)
                                        if(p[pixel[7]] > cb)
                                            if(p[pixel[8]] > cb)
                                                if(p[pixel[9]] > cb)
                                                    if(p[pixel[10]] > cb)
                                                        if(p[pixel[11]] > cb)
                                                            if(p[pixel[13]] > cb)
                                                                if(p[pixel[14]] > cb)
                                                                    if(p[pixel[6]] > cb)
                                                                    {}
                                                                    else
                                                                        if(p[pixel[15]] > cb)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if(p[pixel[12]] < c_b)
                                        if(p[pixel[13]] < c_b)
                                            if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                    if(p[pixel[10]] < c_b)
                                                                        if(p[pixel[11]] < c_b)
                                                                        {}
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                if(p[pixel[11]] > cb)
                                    if(p[pixel[7]] > cb)
                                        if(p[pixel[8]] > cb)
                                            if(p[pixel[9]] > cb)
                                                if(p[pixel[10]] > cb)
                                                    if(p[pixel[12]] > cb)
                                                        if(p[pixel[13]] > cb)
                                                            if(p[pixel[6]] > cb)
                                                                if(p[pixel[5]] > cb)
                                                                {}
                                                                else
                                                                    if(p[pixel[14]] > cb)
                                                                    {}
                                                                    else
                                                                        continue;
                                                            else
                                                                if(p[pixel[14]] > cb)
                                                                    if(p[pixel[15]] > cb)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(p[pixel[11]] < c_b)
                                    if(p[pixel[12]] < c_b)
                                        if(p[pixel[13]] < c_b)
                                            if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                    if(p[pixel[10]] < c_b)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                    if(p[pixel[10]] < c_b)
                                                                    {}
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else
                            if(p[pixel[10]] > cb)
                                if(p[pixel[7]] > cb)
                                    if(p[pixel[8]] > cb)
                                        if(p[pixel[9]] > cb)
                                            if(p[pixel[11]] > cb)
                                                if(p[pixel[12]] > cb)
                                                    if(p[pixel[6]] > cb)
                                                        if(p[pixel[5]] > cb)
                                                            if(p[pixel[4]] > cb)
                                                            {}
                                                            else
                                                                if(p[pixel[13]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                        else
                                                            if(p[pixel[13]] > cb)
                                                                if(p[pixel[14]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                    else
                                                        if(p[pixel[13]] > cb)
                                                            if(p[pixel[14]] > cb)
                                                                if(p[pixel[15]] > cb)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if(p[pixel[10]] < c_b)
                                if(p[pixel[11]] < c_b)
                                    if(p[pixel[12]] < c_b)
                                        if(p[pixel[13]] < c_b)
                                            if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                                if(p[pixel[9]] < c_b)
                                                                {}
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                    else
                        if(p[pixel[9]] > cb)
                            if(p[pixel[7]] > cb)
                                if(p[pixel[8]] > cb)
                                    if(p[pixel[10]] > cb)
                                        if(p[pixel[11]] > cb)
                                            if(p[pixel[6]] > cb)
                                                if(p[pixel[5]] > cb)
                                                    if(p[pixel[4]] > cb)
                                                        if(p[pixel[3]] > cb)
                                                        {}
                                                        else
                                                            if(p[pixel[12]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                    else
                                                        if(p[pixel[12]] > cb)
                                                            if(p[pixel[13]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else
                                                    if(p[pixel[12]] > cb)
                                                        if(p[pixel[13]] > cb)
                                                            if(p[pixel[14]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[12]] > cb)
                                                    if(p[pixel[13]] > cb)
                                                        if(p[pixel[14]] > cb)
                                                            if(p[pixel[15]] > cb)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if(p[pixel[9]] < c_b)
                            if(p[pixel[10]] < c_b)
                                if(p[pixel[11]] < c_b)
                                    if(p[pixel[12]] < c_b)
                                        if(p[pixel[13]] < c_b)
                                            if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[3]] < c_b)
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                            if(p[pixel[8]] < c_b)
                                                            {}
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                else
                    if(p[pixel[8]] > cb)
                        if(p[pixel[7]] > cb)
                            if(p[pixel[9]] > cb)
                                if(p[pixel[10]] > cb)
                                    if(p[pixel[6]] > cb)
                                        if(p[pixel[5]] > cb)
                                            if(p[pixel[4]] > cb)
                                                if(p[pixel[3]] > cb)
                                                    if(p[pixel[2]] > cb)
                                                    {}
                                                    else
                                                        if(p[pixel[11]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                else
                                                    if(p[pixel[11]] > cb)
                                                        if(p[pixel[12]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[11]] > cb)
                                                    if(p[pixel[12]] > cb)
                                                        if(p[pixel[13]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[11]] > cb)
                                                if(p[pixel[12]] > cb)
                                                    if(p[pixel[13]] > cb)
                                                        if(p[pixel[14]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[11]] > cb)
                                            if(p[pixel[12]] > cb)
                                                if(p[pixel[13]] > cb)
                                                    if(p[pixel[14]] > cb)
                                                        if(p[pixel[15]] > cb)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else if(p[pixel[8]] < c_b)
                        if(p[pixel[9]] < c_b)
                            if(p[pixel[10]] < c_b)
                                if(p[pixel[11]] < c_b)
                                    if(p[pixel[12]] < c_b)
                                        if(p[pixel[13]] < c_b)
                                            if(p[pixel[14]] < c_b)
                                                if(p[pixel[15]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[3]] < c_b)
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(p[pixel[2]] < c_b)
                                        if(p[pixel[3]] < c_b)
                                            if(p[pixel[4]] < c_b)
                                                if(p[pixel[5]] < c_b)
                                                    if(p[pixel[6]] < c_b)
                                                        if(p[pixel[7]] < c_b)
                                                        {}
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
            else
                if(p[pixel[7]] > cb)
                    if(p[pixel[8]] > cb)
                        if(p[pixel[9]] > cb)
                            if(p[pixel[6]] > cb)
                                if(p[pixel[5]] > cb)
                                    if(p[pixel[4]] > cb)
                                        if(p[pixel[3]] > cb)
                                            if(p[pixel[2]] > cb)
                                                if(p[pixel[1]] > cb)
                                                {}
                                                else
                                                    if(p[pixel[10]] > cb)
                                                    {}
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[10]] > cb)
                                                    if(p[pixel[11]] > cb)
                                                    {}
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[10]] > cb)
                                                if(p[pixel[11]] > cb)
                                                    if(p[pixel[12]] > cb)
                                                    {}
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[10]] > cb)
                                            if(p[pixel[11]] > cb)
                                                if(p[pixel[12]] > cb)
                                                    if(p[pixel[13]] > cb)
                                                    {}
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(p[pixel[10]] > cb)
                                        if(p[pixel[11]] > cb)
                                            if(p[pixel[12]] > cb)
                                                if(p[pixel[13]] > cb)
                                                    if(p[pixel[14]] > cb)
                                                    {}
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                if(p[pixel[10]] > cb)
                                    if(p[pixel[11]] > cb)
                                        if(p[pixel[12]] > cb)
                                            if(p[pixel[13]] > cb)
                                                if(p[pixel[14]] > cb)
                                                    if(p[pixel[15]] > cb)
                                                    {}
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else
                            continue;
                    else
                        continue;
                else if(p[pixel[7]] < c_b)
                    if(p[pixel[8]] < c_b)
                        if(p[pixel[9]] < c_b)
                            if(p[pixel[6]] < c_b)
                                if(p[pixel[5]] < c_b)
                                    if(p[pixel[4]] < c_b)
                                        if(p[pixel[3]] < c_b)
                                            if(p[pixel[2]] < c_b)
                                                if(p[pixel[1]] < c_b)
                                                {}
                                                else
                                                    if(p[pixel[10]] < c_b)
                                                    {}
                                                    else
                                                        continue;
                                            else
                                                if(p[pixel[10]] < c_b)
                                                    if(p[pixel[11]] < c_b)
                                                    {}
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(p[pixel[10]] < c_b)
                                                if(p[pixel[11]] < c_b)
                                                    if(p[pixel[12]] < c_b)
                                                    {}
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(p[pixel[10]] < c_b)
                                            if(p[pixel[11]] < c_b)
                                                if(p[pixel[12]] < c_b)
                                                    if(p[pixel[13]] < c_b)
                                                    {}
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(p[pixel[10]] < c_b)
                                        if(p[pixel[11]] < c_b)
                                            if(p[pixel[12]] < c_b)
                                                if(p[pixel[13]] < c_b)
                                                    if(p[pixel[14]] < c_b)
                                                    {}
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                if(p[pixel[10]] < c_b)
                                    if(p[pixel[11]] < c_b)
                                        if(p[pixel[12]] < c_b)
                                            if(p[pixel[13]] < c_b)
                                                if(p[pixel[14]] < c_b)
                                                    if(p[pixel[15]] < c_b)
                                                    {}
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else
                            continue;
                    else
                        continue;
                else
                    continue;
            if(num_corners == rsize)
            {
                rsize*=2;
                ret_corners = (xy*)realloc(ret_corners, sizeof(xy)*rsize);
            }
            ret_corners[num_corners].x = x;
            ret_corners[num_corners].y = y;
            num_corners++;

        }

    *ret_num_corners = num_corners;
    return ret_corners;

}

// ----------------------------------------------------------------------------
void vNonlinearFilter(image_t *src,
                      image_t *dst,
                      eFilterOperation fo,
                      uint8_t n) // n equals the mask size
// n should be an odd number
{

    int32_t n_max = (n - 1) / 2;
    int32_t n_min = 0 - n_max;

    register uint32_t x, y;
    register int32_t i, j;

    int n2 = n*n;

    double filterPixelDouble = 0.0;
    int32_t filterPixel = 0;
    int32_t filterPixel2 = 0;

    uint8_t *median;
    if(fo == MEDIAN){
        uint8_t median_list[n2];
        median = &median_list[0];
        filterPixel = n2 /2;
    }

    for(x = n_max; x < dst->width - n_max; x++){
        for(y = n_max; y < dst->height - n_max; y++){
            for(i = n_min; i <= n_max; i++){//Apply filter value for x and y
                for(j = n_min; j <= n_max; j++){
                    switch(fo){
                    case AVERAGE:
                        filterPixel += (src->data[y + j][x + i]);
                        break;

                    case HARMONIC:
                        if(src->data[y + j][x + i] != 0){
                            filterPixelDouble += (1.0/src->data[y + j][x + i]);
                        }
                        break;

                    case MAX:
                        if(src->data[y + j][x + i] > filterPixel)
                            filterPixel = src->data[y + j][x + i];
                        break;

                    case MIN:
                        if(src->data[y + j][x + i] < filterPixel)
                            filterPixel = src->data[y + j][x + i];
                        break;

                    case MIDPOINT:
                    case RANGE:
                        if(src->data[y + j][x + i] > filterPixel)
                            filterPixel = src->data[y + j][x + i];
                        if(src->data[y + j][x + i] < filterPixel2)
                            filterPixel2 = src->data[y + j][x + i];
                        break;

                    case MEDIAN:
                        *median = src->data[y + j][x + i];
                        median++;
                        break;


                    }
                }
            }

            switch(fo){
            case AVERAGE:
                dst->data[y][x] = (uint8_t)(filterPixel / (n2));
                filterPixel = 0;
                break;

            case HARMONIC:
                if(filterPixelDouble != 0.0){
                    dst->data[y][x] = (double)(n2) / filterPixelDouble;
                }else {
                    dst->data[y][x] = 0;
                }
                filterPixelDouble = 0.0;
                break;

            case MAX:
                dst->data[y][x] = filterPixel;
                filterPixel = 0;
                break;

            case MIN:
                dst->data[y][x] = filterPixel;
                filterPixel = 255;
                break;

            case MIDPOINT:
                dst->data[y][x] = (filterPixel + filterPixel2) / 2;
                filterPixel = 0;
                filterPixel2 = 255;
                break;

            case MEDIAN:
                median -= n2; //set median pointer to median[0]
                for(int i=1; i<n2; i++){
                    uint8_t pix = median[i];
                    int j=i-1;
                    while(j>=0 && pix<median[j]){
                        median[j+1] = median[j];
                        j--;
                    }
                    median[j+1] = pix;
                }
                dst->data[y][x] = median[filterPixel];
                break;

            case RANGE:
                dst->data[y][x] = filterPixel - filterPixel2;
                filterPixel = 0;
                filterPixel2 = 255;
                break;
            }
        }
    }

}

// ----------------------------------------------------------------------------
uint32_t iLabelBlobs(image_t *src, // must be a binary image
                     image_t *dst,
                     eConnected connected)
{

    vSetSelectedToValue(src,dst,1,255);

    register uint32_t x,y;

    register uint32_t blobcount = 1;


    for(y=1; y < dst->height; y++){
        for(x=1; x < dst->width; x++){
            if(dst->data[y][x] == 255){
                dst->data[y][x] = blobcount;
                iFillBlob(dst,x,y,255,blobcount,connected);
                blobcount++;
            }
        }
    }

    vSetSelectedToValue(src,dst,255,0);

    return blobcount - 1;

}

void vDetermMinMaxXY(image_t *img, xy_info *pXy, const uint8_t blobcount){
    register uint32_t x,y;
    register uint8_t tmpBlob;

    for(x=0; x < blobcount; x++){
        pXy[x].max_x = 0;
        pXy[x].max_y = 0;
        pXy[x].min_x = img->width;
        pXy[x].min_y = img->height;
    }

    for(y=1; y < img->height; y++){
        for(x=1; x < img->width; x++){
            if(img->data[y][x] != 0){
                tmpBlob = img->data[y][x] - 1;
                if(x < pXy[tmpBlob].min_x ){
                    pXy[tmpBlob].min_x = x;
                }
                if(y < pXy[tmpBlob].min_y ){
                    pXy[tmpBlob].min_y = y;
                }
                if(x > pXy[tmpBlob].max_x ){
                    pXy[tmpBlob].max_x = x;
                }
                if(y > pXy[tmpBlob].max_y ){
                    pXy[tmpBlob].max_y = y;
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
void vBlobAnalyse(image_t *img,
                  const uint8_t blobcount,
                  blobinfo_t *pBlobInfo)
{
    register uint32_t x,y;
    register uint8_t tmpBlob;
    register uint8_t neighbours;
    const double sqrt2 = 1.4142135623;
    const double sqrt5 = 2.2360679774;

    for(x=0; x < blobcount; x++){
        pBlobInfo[x].nof_pixels = 0;
        pBlobInfo[x].width = 0;
        pBlobInfo[x].height = 0;
        pBlobInfo[x].perimeter = 0;
        pBlobInfo[x].nof_corners = 0;
        pBlobInfo[x].min_x = img->width;
        pBlobInfo[x].min_y = img->height;
        pBlobInfo[x].max_x = 0;
        pBlobInfo[x].max_y = 0;
    }

    for(y=1; y < img->height; y++){
        for(x=1; x < img->width; x++){
            if(img->data[y][x] != 0){
                tmpBlob = img->data[y][x] - 1;
                pBlobInfo[tmpBlob].nof_pixels++;
                if(x < pBlobInfo[tmpBlob].min_x){
                    pBlobInfo[tmpBlob].min_x = x;
                }
                if(y < pBlobInfo[tmpBlob].min_y ){
                    pBlobInfo[tmpBlob].min_y = y;
                }
                if(x > pBlobInfo[tmpBlob].max_x){ //Max X
                    pBlobInfo[tmpBlob].max_x = x;
                }
                if(y > pBlobInfo[tmpBlob].max_y){ //Max Y
                    pBlobInfo[tmpBlob].max_y = y;
                }
            }
        }
    }

    for(x=0; x < blobcount; x++){
        pBlobInfo[x].width = pBlobInfo[x].max_x - pBlobInfo[x].min_x;
        pBlobInfo[x].height = pBlobInfo[x].max_y - pBlobInfo[x].min_y;
        //pBlobInfo[x].perimeter = (pBlobInfo[x].perimeter * pBlobInfo[x].perimeter) / pBlobInfo[x].nof_pixels;
    }
}


void vBlobBoundingCornerDetect(image_t *img, xy_info *xy_locs,
                               uint8_t b,
                               blobinfo_t *pBlobInfo){
    register uint32_t i;
    u_int32_t cornerC[4] = {0 ,0 , 0 ,0};

    xy_locs[0].min_x = img->width;
    xy_locs[0].max_x = 0;
    xy_locs[1].min_x = img->width;
    xy_locs[1].max_x = 0;
    xy_locs[2].min_y = img->height;
    xy_locs[2].max_y = 0;
    xy_locs[3].min_y = img->height;
    xy_locs[3].max_y = 0;

    for(i = pBlobInfo->min_x; i <= pBlobInfo->max_x; i++)
    {
        if(img->data[pBlobInfo->min_y][i] == b){
            img->data[pBlobInfo->min_y][i] = 70;
            if(i < xy_locs[0].min_x){
                xy_locs[0].min_x = i;
            }
            if(i > xy_locs[0].max_x){
                xy_locs[0].max_x = i;
            }
            xy_locs[0].min_y = pBlobInfo->min_y;
            xy_locs[0].max_y = pBlobInfo->min_y;
            cornerC[0] = 1;
        }
    }

    for(i = pBlobInfo->min_x; i <= pBlobInfo->max_x; i++){
        if(img->data[pBlobInfo->max_y][i] == b){
            img->data[pBlobInfo->max_y][i] = 70;
            if(i < xy_locs[1].min_x){
                xy_locs[1].min_x = i;
            }
            if(i > xy_locs[1].max_x){
                xy_locs[1].max_x = i;
            }
            xy_locs[1].min_y = pBlobInfo->max_y;
            xy_locs[1].max_y = pBlobInfo->max_y;
            cornerC[1] = 1;
        }
    }

    for(i = pBlobInfo->min_y; i <= pBlobInfo->max_y; i++)
    {
        if(img->data[i][pBlobInfo->min_x] == b){
            img->data[i][pBlobInfo->min_x] = 70;
            xy_locs[2].min_x = pBlobInfo->min_x;
            xy_locs[2].max_x = pBlobInfo->min_x;
            if(i < xy_locs[2].min_y){
                xy_locs[2].min_y = i;
            }
            if(i > xy_locs[2].max_y){
                xy_locs[2].max_y = i;
            }
            cornerC[2] = 1;
        }
    }
    for(i = pBlobInfo->min_y; i <= pBlobInfo->max_y; i++)
    {
        if(img->data[i][pBlobInfo->max_x] == b){
            img->data[i][pBlobInfo->max_x] = 70;
            xy_locs[3].min_x = pBlobInfo->max_x;
            xy_locs[3].max_x = pBlobInfo->max_x;
            if(i < xy_locs[3].min_y){
                xy_locs[3].min_y = i;
            }
            if(i > xy_locs[3].max_y){
                xy_locs[3].max_y = i;
            }
            cornerC[3] = 1;
        }
    }

    pBlobInfo->nof_corners = cornerC[0] + cornerC[1] + cornerC[2] + cornerC[3];

    /*img->data[xy_locs[0].y][xy_locs[0].x] = 70;
    img->data[xy_locs[1].y][xy_locs[1].x] = 70;
    img->data[xy_locs[2].y][xy_locs[2].x] = 70;
    img->data[xy_locs[3].y][xy_locs[3].x] = 70;*/

}


double iCentralMoment(image_t *img, uint8_t blobnr, int p, int q, int xc, int yc){
    register int32_t x,y;
    double moment = 0.0;


    for(y=0; y < img->height; y++){
        for(x=0; x < img->width; x++){
            if(img->data[y][x] == blobnr){
                moment += pow((x-xc),p) * pow((y-yc),q); // * img->data[y][x]; //slow method
            }
        }
    }
    return moment;
}


int iBasicMoment(image_t *img, uint8_t blobnr, int p, int q){
    register uint32_t x,y;
    register uint32_t moment = 0;


    for(y=0; y < img->height; y++){
        for(x=0; x < img->width; x++){
            if(img->data[y][x] == blobnr){
                //moment += pow(x,p) * pow(y,q) * img->data[y][x]; //slow method

                if(p == 1 && q == 1){ //p = 1, q = 1, X * Y * 1
                    moment += x * y;
                } else if(p == 1 && q == 0){ //p = 1, q = 0, X * 1 * 1
                    moment += x;
                } else if (q == 1){ //p = 0, q = 1, 1 * Y * 1
                    moment += y;
                } else { //p and q are 0, so we got 1 * 1 * 1
                    moment += 1;
                }
            }
        }
    }
    return moment;
}

void vCentroid(image_t *img, uint8_t blobnr, uint16_t *xc, uint16_t *yc)
{
    register uint32_t m10, m01, m00;

    m00 = iBasicMoment(img,blobnr,0,0);
    m01 = iBasicMoment(img,blobnr,0,1);
    m10 = iBasicMoment(img,blobnr,1,0);

    if(m00 != 0){
        *xc = (uint16_t)(m10 / m00);
        *yc = (uint16_t)(m01 / m00);
    }else{
        qDebug() << "FPE error";
    }

}

double dNormalizedCentralMoments(image_t *img, uint8_t blobnr, int p, int q)
{
    if((p == 0 && q == 1) || (p == 1 && q == 0)){
        return 0.0;
    }

    if(p == 0 && q == 0){
        return 1.0;
    }

    uint16_t xc, yc;
    double upq, u00, ncm;

    vCentroid(img,blobnr,&xc,&yc);

    upq = iCentralMoment(img,blobnr,p,q,xc,yc);
    u00 = iCentralMoment(img,blobnr,0,0,0,0);

    ncm = upq / (u00 * u00);

    return ncm;
}

void vLDF(image_t *src,
          image_t *dst)
{

#warning TODO: vLDF
    // ********************************************
    // Added to prevent compiler warnings
    // Remove these when implementation starts
    src=src;
    dst=dst;

    return;
    // ********************************************

}

void iFillBlob(image_t *img,
               uint16_t x,
               uint16_t y,
               uint8_t value,
               uint8_t fill,
               eConnected connected){

    if(img->data[y-1][x] == value){
        img->data[y-1][x] = fill;
        iFillBlob(img,x,y-1,value,fill,connected);
    }

    if(img->data[y+1][x] == value) {
        img->data[y+1][x] = fill;
        iFillBlob(img,x,y+1,value,fill,connected);
    }

    if(img->data[y][x-1] == value) {
        img->data[y][x-1] = fill;
        iFillBlob(img,x-1,y,value,fill,connected);
    }

    if(img->data[y][x+1] == value) {
        img->data[y][x+1] = fill;
        iFillBlob(img,x+1,y,value,fill,connected);
    }

    if(connected == EIGHT){
        if(img->data[y-1][x-1] == value){
            img->data[y-1][x-1] = fill;
            iFillBlob(img,x-1,y-1,value,fill,connected);
        }

        if(img->data[y-1][x+1] == value){
            img->data[y-1][x+1] = fill;
            iFillBlob(img,x+1,y-1,value,fill,connected);
        }

        if(img->data[y+1][x-1] == value) {
            img->data[y+1][x-1] = fill;
            iFillBlob(img,x-1,y+1,value,fill,connected);
        }

        if(img->data[y+1][x+1] == value) {
            img->data[y+1][x+1] = fill;
            iFillBlob(img,x+1,y+1,value,fill,connected);
        }
    }

}



//Note make sure, you are not getting out of bounds
//e.g 0,1 is wrong
uint8_t iNeighbourCount(image_t *img,
                        uint16_t x,
                        uint16_t y,
                        uint8_t value,
                        eConnected connected)
{
    register uint8_t nCount = 0;

    //Check four connected
    // 0 * 0
    // * 0 *
    // 0 * 0
    if(img->data[y-1][x] == value)
        nCount++;

    if(img->data[y+1][x] == value)
        nCount++;

    if(img->data[y][x-1] == value)
        nCount++;

    if(img->data[y][x+1] == value)
        nCount++;


    //Check eight connected
    // * 0 *
    // 0 0 0
    // * 0 *
    if(connected == EIGHT){
        if(img->data[y-1][x-1] == value)
            nCount++;

        if(img->data[y-1][x+1] == value)
            nCount++;

        if(img->data[y+1][x-1] == value)
            nCount++;

        if(img->data[y+1][x+1] == value)
            nCount++;
    }

    return nCount;
}

uint8_t iNeighboursEqualOrHigher(image_t *img,
                                 uint16_t x,
                                 uint16_t y,
                                 uint8_t value,
                                 eConnected connected)
{
    register uint8_t nCount = 0;

    //Check four connected
    // 0 * 0
    // * 0 *
    // 0 * 0
    if(img->data[y-1][x] >= value)
        nCount++;

    if(img->data[y+1][x] >= value)
        nCount++;

    if(img->data[y][x-1] >= value)
        nCount++;

    if(img->data[y][x+1] >= value)
        nCount++;


    //Check eight connected
    // * 0 *
    // 0 0 0
    // * 0 *
    if(connected == EIGHT){
        if(img->data[y-1][x-1] >= value)
            nCount++;

        if(img->data[y-1][x+1] >= value)
            nCount++;

        if(img->data[y+1][x-1] >= value)
            nCount++;

        if(img->data[y+1][x+1] >= value)
            nCount++;
    }

    return nCount;
}


uint8_t iNeighboursEqualOrLower(image_t *img,
                                uint16_t x,
                                uint16_t y,
                                uint8_t value,
                                eConnected connected)
{
    register uint8_t nCount = 0;

    //Check four connected
    // 0 * 0
    // * 0 *
    // 0 * 0
    if(img->data[y-1][x] <= value)
        nCount++;

    if(img->data[y+1][x] <= value)
        nCount++;

    if(img->data[y][x-1] <= value)
        nCount++;

    if(img->data[y][x+1] <= value)
        nCount++;


    //Check eight connected
    // * 0 *
    // 0 0 0
    // * 0 *
    if(connected == EIGHT){
        if(img->data[y-1][x-1] <= value)
            nCount++;

        if(img->data[y-1][x+1] <= value)
            nCount++;

        if(img->data[y+1][x-1] <= value)
            nCount++;

        if(img->data[y+1][x+1] <= value)
            nCount++;
    }

    return nCount;
}


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
