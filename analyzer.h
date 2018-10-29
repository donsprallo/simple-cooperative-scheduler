/* 
 * File:   analyzer.h
 * Author: nico
 *
 * Created on 3. Mai 2018, 10:45
 */


#include <stdint.h>
#include <stdlib.h>

#include "../os/scheduler.h"
#include "../com/uart.h"


#ifndef SCHEDULER_ANALYZER_H
#define	SCHEDULER_ANALYZER_H

#ifdef	__cplusplus
extern "C" {
#endif


typedef struct task_measure_t
{
    // identifier
    uint8_t id;
    // measured task
    task_t* task;
    // measure value
    uint32_t measure;
    // next element
    struct task_measure_t* next;
} task_measure_t;


// read the 32-bit timer value
uint32_t static inline _read_measure_timer(void)
{
    // read timer
    uint16_t lsw = TMR2;
    uint16_t msw = TMR3HLD;
    
    return ((uint32_t)msw << 16 | (uint32_t)lsw);
};


// reset the 32-bit timer value
void static inline _reset_measure_timer(void)
{
    TMR3HLD = 0;
    TMR2 = 0;
};


// scheduler measure task priority
#define SCHEDULER_MEASURE_PRIORITY 1

// scheduler measure task interval
#ifdef DEBUG
#define SCHEDULER_MEASURE_INTERVAL 50
#else
#define SCHEDULER_MEASURE_INTERVAL 1000
#endif


// enable task time analyzing
// return 1 when enable is success, else 0
uint8_t analyzer_enable(void);


// disable the task time analyzing
void inline analyzer_disable(void);


// add a task to measure by analyzer
task_measure_t* analyzer_measure_task(task_t* task, uint8_t id);


// mesaure function
void _analyzer_measure(void* wrapper, uint8_t state);


// scheduler measurement handler
void _analyze_handler(task_t* task, void* param);


#ifdef	__cplusplus
}
#endif

#endif	/* SCHEDULER_ANALYZER_H */

