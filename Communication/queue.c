#include "queue.h"

void queue_init(volatile Queue * pQueue)
{
    memset(pQueue, 0, sizeof(Queue));
}

char queue_get(volatile Queue * pQueue)
{
    char data = pQueue->arr[pQueue->head];
    pQueue->head = (pQueue->head + 1) % QUEUE_SIZE;
    return data;
}

void queue_put(volatile Queue * pQueue, char ch)
{
    pQueue->arr[pQueue->tail] = ch;
	pQueue->tail = (pQueue->tail + 1) % QUEUE_SIZE;
}

int queue_empty(volatile Queue queue)
{
    if(queue.head == queue.tail)
        return 1;
    return 0;
}

int queue_full(volatile Queue queue)
{
    if((queue.tail + 1) % QUEUE_SIZE == queue.head)
        return 1;
    return 0;
}
