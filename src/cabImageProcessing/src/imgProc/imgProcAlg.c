#include "imgProcAlg.h"
#include <math.h>
#include <sys/printk.h>

int guideLineSearch(Image img, int16_t *pos, float *angle)
{
    int i, gf_pos;

    /* Inits */
    *pos = -1;
    gf_pos = -1;

    /* Search for guideline pos - Near*/
    for (i = 0; i < IMGWIDTH; i++)
    {
        if (img.data[i] == GUIDELINE_COLOR)
        {
            *pos = i;
            break;
        }
    }

    /* Search for guideline pos - Far*/
    for (i = 0; i < IMGWIDTH; i++)
    {
        if (img.data[IMGWIDTH * (IMGWIDTH - 1) + i] == GUIDELINE_COLOR)
        {
            gf_pos = i;
            break;
        }
    }

    if (*pos == -1 || gf_pos == -1)
    {
        printk("Failed to find guideline pos=%d, gf_pos=%d\n", *pos, gf_pos);
        return -1;
    }

    /* Approach very grossly the angle (NOT a valid solution - just for testing ) */
    if (*pos == gf_pos)
    {
        *angle = 0;
    }
    else
    {
        int a = *pos - (IMGWIDTH / 2);
        if (a < 0)
        {
            a = -a;
        }
        *angle = (a * M_PI_4) / (IMGWIDTH / 2);
    }
    *pos = 100 - ((*pos * 100) / IMGWIDTH);

    return 0;
}

/* Function to look for close by obstacles */
int nearObstSearch(Image img)
{
    int i, j, n_obs, visited;

    /* Inits */
    visited = 0;
    n_obs = 0;

    /* Search for obstacles */
    for (j = CSA_ROW; j < IMGWIDTH; j++)
    {
        for (i = CSA_COL_L; i < CSA_COL_R; i++)
        {
            if (img.data[IMGWIDTH * j + i] == OBSTACLE_COLOR && visited == 0)
            {
                n_obs++;
                visited = 1;
            }
            else if (img.data[IMGWIDTH * j + i] == BACKGROUND_COLOR)
            {
                visited = 0;
            }
        }
    }
    return n_obs;
}

/* Function that counts obstacles.*/
int obstCount(Image img)
{
    int i, j, n_obs, visited;

    /* Inits */
    visited = 0;
    n_obs = 0;

    /* Search for obstacles */
    for (j = 0; j < IMGWIDTH; j++)
    {
        for (i = 0; i < IMGWIDTH; i++)
        {
            if (img.data[IMGWIDTH * j + i] == OBSTACLE_COLOR && visited == 0)
            {
                n_obs++;
                visited = 1;
            }
            else if (img.data[IMGWIDTH * j + i] == BACKGROUND_COLOR)
            {
                visited = 0;
            }
        }
    }

    return n_obs;
}

void printImage(Image img)
{
    printk("Image matrix:\n");

    int i, j;
    for (j = 0; j < IMGWIDTH; j++)
    {
        for (i = 0; i < IMGWIDTH; i++)
        {
            printk("\t%d", img.data[IMGWIDTH * j + i]);
        }
        printk("\n\r");
    }
}