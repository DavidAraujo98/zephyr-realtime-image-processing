#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include "imageGenerator.h"

Image createImage()
{
    Image img;

    time_t t;
    srand((unsigned)time(&t));
    int deviation_t = rand() % (IMGWIDTH - 1);
    int deviation_b = (IMGWIDTH / 2) + (IMGWIDTH / 2 - deviation_t) - 1;

    int numObstacles = rand() % 3 + 2;

    for (int n = 0; n < numObstacles; n++){
        // Randomly determine the size of the obstacle
        int obstacleSize = rand() % (IMGWIDTH - 2) + 2;
        int obstacleRow = rand() % (IMGWIDTH - 2) + 1;
        int obstacleStart = rand() % (IMGWIDTH - obstacleSize);

        for (int i = 0; i < IMGWIDTH; i++)
        {
            for (int j = 0; j < IMGWIDTH; j++)
            {
                if (n > 0 && img.data[i * IMGWIDTH + j] == OBSTACLE_COLOR)
                    continue;
                    
                if ((j == deviation_t && i == 0) || (j == deviation_b && i == IMGWIDTH - 1))
                {
                    img.data[i * IMGWIDTH + j] = GUIDELINE_COLOR;
                }
                else if (i == obstacleRow && (j >= obstacleStart && j < obstacleStart + obstacleSize))
                {
                    img.data[i * IMGWIDTH + j] = OBSTACLE_COLOR;
                }
                else
                {
                    img.data[i * IMGWIDTH + j] = BACKGROUND_COLOR;
                }
            }
        }
        sleep(1);
    }

    return img;
}

// Creates a folder with a provided name, and X random images in that folder
void createImageFolder()
{
    for (int i = 1; i <= DIRSIZE; i++)
    {
        char fileName[20];
        sprintf(fileName, "images/img%d.raw", i);

        FILE *fp = fopen(fileName, "w");

        Image img = createImage();
        for (int i = 0; i < IMGWIDTH; i++)
        {
            for (int j = 0; j < IMGWIDTH; j++)
            {
                fprintf(fp, "%#x ", img.data[i * IMGWIDTH + j]);
            }
            fprintf(fp, "\r");
        }
        fclose(fp);
        sleep(1); // This need to be done because the randomness in the images is time dependent
    }
}