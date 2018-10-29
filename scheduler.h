/*
* scheduler.h
*
* Created: 08.04.2018
* Author: Nico Hanisch
*/


#include <stdint.h>
#include <stdlib.h>

#include "../time/systimer.h"


#ifndef SCHEDULER_SCHEDULER_H
#define SCHEDULER_SCHEDULER_H

#ifdef	__cplusplus
extern "C" {
#endif


// define a state type for tasks
typedef enum
{
    // task is started
    STARTED,
    // task is stopped
    STOPPED,
    // task is running
    RUNNING,
    // task is suspended
    SUSPENDED,
    // task is waiting
    WAITING
} task_state_t;


// define a task
typedef struct task_t
{
    // task handle
    int handle;
    // task state
    task_state_t state;
    // last start timestamp
	clock_t last_start;
    // sheduling interval
	uint16_t period;
    // scheduling priority (1 - lowest, 255 - highest)
	uint8_t priority;
    // function pointer for task handler
	void (*handler_callback)(struct task_t*, void*);
    // task parameter
    void* param;
    // suspend time
    uint16_t suspend;
    // function handler for task measurement
    void (*measure_callback)(void*, uint8_t);
    // measure type wrapper
    void* measure_wrapper;
} task_t;



// scheduler task size
#define SCHEDULER_TASK_SIZE (sizeof(task_t))


// define a element for a forwarded list
typedef struct scheduler_node_t
{
    // node caption
    task_t* content;
    // pointer to next node in forwarded list
    struct scheduler_node_t* next;
} scheduler_node_t;


// scheduler node size
#define SCHEDULER_NODE_SIZE (sizeof(scheduler_node_t))


// timer function pointer
typedef clock_t (*timer_ptr) ();


// task function pointer
typedef void (*task_ptr) (task_t*, void*);


// inititalize the scheduler
// return 1 when success, else 0
uint8_t scheduler_init(timer_ptr timer);


// Create a task for the scheduler
// return the task pointer when success, else NULL
task_t *scheduler_create_task(
        uint8_t priority,
        clock_t period,
        task_ptr handler,
        void *param);


// Add a task to the priority ordered list
// return 1 when add success, else 0
uint8_t scheduler_add_task(task_t* task);


// Remove a task from scheduler
void scheduler_remove_task(task_t* task);


// Kill task with handle
void scheduler_kill_task(int handle);


// Begin the scheduling in an infinite loop
void scheduler_run();


// suspend task from scheduling for a time
void static inline scheduler_suspend_task(task_t* task, clock_t ms)
{
    task->state = SUSPENDED;
    task->suspend = ms;
};


// start task
void static inline scheduler_start_task(task_t* task) {
    task->state = STARTED;
};


// stop task
void static inline scheduler_stop_task(task_t* task) {
    task->state = STOPPED;
};


// set task period
void static inline scheduler_set_task_period(task_t* task, clock_t period) {
    task->period = period;
}


// get task period
clock_t static inline scheduler_get_task_period(task_t* task) {
    return task->period;
}


int static inline scheduler_get_handle(task_t* task) {
    return task->handle;
}


// This is the scheduler-IDLE function
void static inline _idle_handler(task_t* task, void* param) {
    // showing idle mode on pin
#if 0
    asm("btg LATB,#0");
#else
    asm("nop");
#endif
};


// run a task, should only used by scheduler
void _scheduler_run_task(task_t* task, clock_t* now);


#ifdef	__cplusplus
}
#endif

#endif //SCHEDULER_SCHEDULER_H