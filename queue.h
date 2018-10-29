/* 
 * File:   queue.h
 * Author: Nico Hanisch
 *
 * Created on 2. Mai 2018, 14:29
 */

#include <stdint.h>
#include <stdlib.h>
#include <xc.h>


#ifndef QUEUE_QUEUE_H
#define	QUEUE_QUEUE_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    
// disable global interrupts
#define ENTER_CRITICAL_SECTION asm("bclr INTCON2, #15");
    
// enable global interrupts
#define LEAVE_CRITICAL_SECTION asm("bset INTCON2, #15");

// queue-data type
typedef char queue_data_t;

// queue-data pointer type
typedef char* queue_data_ptr;


// queue type
typedef struct
{
	queue_data_ptr start;
	queue_data_ptr head;
	queue_data_ptr tail;
	uint32_t size;
	uint32_t count;
} queue_t, *queue_ptr;


/**
 * Initialize the queue with reserved size.
 * @param size Reserved size of queue-data type.
 * @return Struct of initialized and allocated queue.
 */
queue_ptr queue_create(uint32_t size);


/**
 * Push data into the queue.
 * @param queue An inizialized and allocated queue.
 * @param data Data to push into the queue when succeed.
 * @return 1 when push succeed, else 0.
 */
uint8_t queue_push(queue_ptr queue, queue_data_ptr data);


/**
 * Get data from the queue and remove the data.
 * @param queue An initialized and allocated queue.
 * @param data Data popped from the queue when succeed.
 * @return 1 when pop succeed, else 0.
 */
uint8_t queue_pop(queue_ptr queue, queue_data_ptr data);


/**
 * Check if data is in queue.
 * @param queue An initialized and allocated queue.
 * @return The queue data count.
 */
uint8_t queue_available(queue_ptr queue);


/**
 * Clear all data from the queue.
 * @param queue An initialized and allocated queue.
 */
void queue_clear(queue_ptr queue);


/**
 * Delete reserved queue memory.
 * @param fifo An initialized and allocated queue.
 */
void queue_delete(queue_ptr queue);


#ifdef	__cplusplus
}
#endif

#endif	/* QUEUE_QUEUE_H */

