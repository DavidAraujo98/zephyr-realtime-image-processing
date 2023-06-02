#ifndef __CAB__
#define __CAB__

typedef struct CAB
{
    char *name;
    int num;
    int dim;
    void *first;
    void *data;
} CAB;

CAB *open_cab(char *name, int num, int dim, void *first);

void *reserve(CAB *cabId);

void put_mes(CAB *cabId, void *buffer);

void *get_mes(CAB *cabId);

void unget(void *mes_pointer, CAB *cabId);

#endif
