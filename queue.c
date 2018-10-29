#include "queue.h"


queue_ptr queue_create(uint32_t size)
{
	queue_ptr queue =
            (queue_ptr)malloc(sizeof(queue_t));

	// allocation success
	if (queue != NULL)
	{
		queue_data_t* data =
                (queue_data_ptr)malloc(sizeof(queue_data_t) * size);

		// allocation success
		if (data != NULL)
		{
			queue->count = 0;
			queue->head  = data;
			queue->tail  = data;
			queue->start = data;
			queue->size  = size;
            
            return queue;
		}

		// allocation fail
		else
		{
			free(queue);

			return NULL;
		}
	}

	// allocation fail
	else
		return NULL;
}


uint8_t queue_push(queue_ptr queue, queue_data_ptr data)
{
	uint8_t success = 0;
    
    ENTER_CRITICAL_SECTION

	// head and tail position ok
	if (queue->head != queue->tail || queue->count == 0)
	{
		// push data
		*queue->head = *data;
		queue->count++;
		success = 1;

		// head cycle detected
		if ((queue->start + queue->size - 1) == queue->head)
			queue->head = queue->start;
		
		// head step
		else
			queue->head++;
	}
    
    LEAVE_CRITICAL_SECTION

	return success;
}


uint8_t queue_pop(queue_ptr queue, queue_data_ptr data)
{
	uint8_t success = 0;
    
    ENTER_CRITICAL_SECTION

	// head and tail position ok
	if (queue->head != queue->tail || queue->count == queue->size)
	{
		// pop data
		*data = *queue->tail;
		queue->count--;
		success = 1;

		// tail cycle detected
		if ((queue->start + queue->size - 1) == queue->tail)
			queue->tail = queue->start;

		// tail step
		else
			queue->tail++;
	}
    
    LEAVE_CRITICAL_SECTION

	return success;
}


uint8_t inline queue_available(queue_ptr queue)
{
	return queue->count;
}


void queue_clear(queue_ptr queue)
{
	// reset queue
	queue->count = 0;
	queue->head  = queue->start;
	queue->tail  = queue->start;
}


void queue_delete(queue_ptr queue)
{
	// deallocate queue
	if (queue != NULL)
	{
		if (queue->start != NULL)
		{
			free(queue->start);
			queue->start = NULL;
			queue->head  = NULL;
			queue->tail  = NULL;
		}

		free(queue);
		queue = NULL;
	}
}