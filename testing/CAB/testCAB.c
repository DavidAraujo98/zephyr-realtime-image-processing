#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "cab.h"

// Global CAB struct
int initial_msg;
CAB *cab;

// Thread function that reads from the CAB
void *thread_func_1(void *arg)
{
    while (1)
    {
        // Get a message from the CAB
        int *val = get_mes(cab);
        if (val != NULL)
        {
            // Print the value from the message
            printf("Task 1: value = %d\n", *val);
        }

        sleep(2);
    }

    return NULL;
}

// Thread function that reads from the CAB
void *thread_func_2(void *arg)
{
    while (1)
    {
        // Get a message from the CAB
        int *val = get_mes(cab);
        if (val != NULL)
        {
            // Print the value from the message
            printf("Task 2: value = %d\n", *val);
        }

        sleep(2);
    }

    return NULL;
}

int main(void)
{
    // Create a CAB with name "test", 5 elements, element size of 4 bytes, and the initial message
    cab = open_cab("test", 5, 4, &initial_msg);

    // Populate the CAB with a value
    int *val = reserve(cab);
    if (val == NULL)
    {
        printf("Failed to reserve buffer from CAB");
        exit(0);
    }
    // Put a value into the buffer
    *val = 1;
    // Put the buffer into the CAB
    put_mes(cab, val);

    // Create threads
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, thread_func_1, NULL);
    pthread_create(&thread2, NULL, thread_func_2, NULL);

    int j = 1;
    while(1){
        j++;

        if (j > 10)
            j = 1;

        // Unget the initial message from the CAB
        *val = get_mes(cab);
        unget(val, cab);

        // Put a value into the buffer
        *val = reserve(cab);
        *val = j;
        put_mes(cab, val);

        sleep(10);
    }

    // Wait for threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Free memory
    free(cab->data);
    free(cab);

    return 0;
}
