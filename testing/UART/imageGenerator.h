#ifndef __IMAGEGENERATOR__
#define __IMAGEGENERATOR__

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

typedef struct
{
    unsigned char data[IMGWIDTH * IMGWIDTH];
} Image;

Image createImage();

int createImageFolder();

#endif