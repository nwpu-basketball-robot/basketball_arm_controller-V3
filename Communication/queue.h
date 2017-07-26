#ifndef   QUEUE_H_
#define   QUEUE_H_

#include <string.h>
 
#define QUEUE_SIZE 129
typedef struct{
    char arr[QUEUE_SIZE];
    int head,tail;
}Queue;

void queue_init(volatile Queue * pQueue);
char queue_get(volatile Queue * pQueue);
void queue_put(volatile Queue * pQueue, char ch);
int queue_empty(volatile Queue queue);
int queue_full(volatile Queue queue);


#endif
