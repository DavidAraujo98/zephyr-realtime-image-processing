#ifndef __IMAGEPROCESSOR__
#define __IMAGEPROCESSOR__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define IMGWIDTH 16
#define BACKGROUND_COLOR 0x00
#define GUIDELINE_COLOR 0xFF
#define OBSTACLE_COLOR 0x80
#define GN_ROW 0
#define GF_ROW (IMGWIDTH - 1)
#define CSA_ROW (IMGWIDTH / 2)
#define CSA_COL_L (IMGWIDTH / 4)
#define CSA_COL_R (3 * IMGWIDTH / 4)
#define M_PI_4 0.78539816339744830962

typedef struct
{
    unsigned char data[IMGWIDTH * IMGWIDTH];
} Image;

int guideLineSearch(Image img, int16_t *pos, float *angle);

int nearObstSearch(Image img);

int obstCount(Image img);

void printImage(Image img);

#endif