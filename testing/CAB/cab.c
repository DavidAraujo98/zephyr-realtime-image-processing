#include <zephyr.h>
#include <sys/printk.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cab.h"

CAB *open_cab(char *name, int num, int dim, void *first)
{
    // Allocate memory for the CAB struct
    CAB *cab = malloc(sizeof(CAB));
    if (cab == NULL)
    {
        printk("Failed to allocate memory for CAB struct\n", LOG_LEVEL_DBG);
        return NULL;
    }

    // Initialize the fields of the CAB struct
    cab->name = name; // make a copy of the name string
    cab->num = num;
    cab->dim = dim;
    cab->first = first; // modified to store the initial message

    // Allocate memory for the data storage of the CAB
    cab->data = malloc(num * dim);
    if (cab->data == NULL)
    {
        printk("Failed to allocate memory for CAB data\n", LOG_LEVEL_DBG);
        free(cab);
        return NULL;
    }

    // Return the pointer to the CAB struct
    return cab;
}

// Reserve a buffer from the CAB's memory space
void *reserve(CAB *cabId)
{
    // Check if the CAB is full
    if (cabId->first == NULL)
    {
        // The "first" field is NULL, so there is no initial message in the CAB
        return NULL;
    }
    else
    {
        // Return a pointer to the initial message
        return cabId->first;
    }
}

// Copy a message into a buffer reserved from the CAB's memory space and
// put the buffer into the CAB structure
void put_mes(CAB *cabId, void *buffer)
{
    // Store the buffer as the initial message in the CAB
    // (we assume that the "first" field is NULL, indicating that there is no initial message in the CAB)
    cabId->first = buffer;
}

void *get_mes(CAB *cabId)
{
    // Check if the CAB is empty
    if (cabId->first == NULL)
    {
        // The "first" field is NULL, so there is no initial message in the CAB
        printk("CAB is empty\n", LOG_LEVEL_DBG);
        return NULL;
    }
    else
    {
        // Return a pointer to the initial message
        return cabId->first;
    }
}

// Release a pointer to a message in a CAB
void unget(void *mes_pointer, CAB *cabId)
{
    // Check if the pointer points to the initial message in the CAB
    if (mes_pointer != cabId->first)
    {
        // The pointer does not point to the initial message, so it is invalid
        printk("Invalid pointer\n", LOG_LEVEL_DBG);
        return;
    }

    // Clear the initial message by setting the "first" field to NULL
    cabId->first = NULL;
}