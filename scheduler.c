/*
* scheduler.c
*
* Created: 08.04.2018
* Author: Nico Hanisch
*/


#include "scheduler.h"


// timer instance
timer_ptr _timer = NULL;

// root element of tasks
static scheduler_node_t* _root = NULL;


uint8_t scheduler_init(timer_ptr systimer)
{
	// create and insert the IDLE task
	task_t* idle_task = scheduler_create_task(
            0, 0, &_idle_handler, NULL);
	
	if (idle_task != NULL)
    {
        if (scheduler_add_task(idle_task))
        {
            _timer = systimer;
            
            // init success
            return 1;
        }
        
        // init fail
        else
            return 0;
    }
    
    // init fail
    else
        return 0;
}


task_t* scheduler_create_task(
    uint8_t priority,
    clock_t period,
    task_ptr handler,
    void *param)
{
    static int id = 0;
	task_t *new_task = (task_t*)malloc(sizeof(task_t));

	if (new_task != NULL)
	{
		// create the task
        new_task->handle = id;
        new_task->state = STOPPED;
		new_task->last_start = 0;
		new_task->priority = priority;
		new_task->period = period;
		new_task->handler_callback = handler;
        new_task->param = param;
        new_task->suspend = 0;
        new_task->measure_callback = NULL;
        
        id++;
        
		return new_task;
	}
	
	else
        return NULL;
}


void scheduler_remove_task(task_t* task)
{
	scheduler_node_t* cur = _root;
    scheduler_node_t* last = cur;
	
	// find node to remove
	do
	{
        // removeable task found
		if (cur->content == task)
		{
            // if current task is root
            if (cur->content == _root->content)
                _root = cur->next;
            
            else
            {
                // link last node to next
                last->next = cur->next;
            }
            
			free(cur->content);
			free(cur);
            
            cur = NULL;
		}
		
		else
        {
            // set next node
            last = cur;
			cur = cur->next;
        }
        
	} while(cur);
}


void scheduler_kill_task(int handle)
{
    scheduler_node_t* cur = _root;
    
    while (cur)
    {
        if (cur->content->handle == handle)
            scheduler_remove_task(cur->content);
        
        else
            cur = cur->next;
    }
}


void _scheduler_run_task(task_t* task, clock_t* now)
{
    // start measure
    if (task->measure_callback != NULL)
        task->measure_callback(task->measure_wrapper, 0);
    
    // make callback
    task->last_start = *now;
    task->state = RUNNING;
    task->handler_callback(task, task->param);
    
    // end measure
    if (task->measure_callback != NULL)
        task->measure_callback(task->measure_wrapper, 1);

    // if state is not changed at callback
    // then return to state STARTED
    if (task->state == RUNNING)
        task->state =  STARTED;
}


void scheduler_run()
{
	scheduler_node_t *cur = _root;
	
	while (1)
	{
		// get time
		clock_t now = _timer();
        
        switch(cur->content->state)
        {
            case STARTED:
            {
                // check ticks
                if ((now - cur->content->last_start) >= cur->content->period)
                {
                    _scheduler_run_task(cur->content, &now);
                    
                    // begin new loop
                    cur = _root;
                }
                
                else
                {
                    // set to next element
                    cur = cur->next;
                }
            }
            break;
            
            
            case STOPPED:
            {
                // set to next element
                cur = cur->next;
            }
                break;
            
                
                // fail state
            case RUNNING:
                break;
                
                
            case SUSPENDED:
            {                
                // change task state
                if ((now - cur->content->last_start) >= cur->content->suspend)
                {
                    _scheduler_run_task(cur->content, &now);
                    
                    // begin new loop
                    cur = _root;
                }
                
                else
                {
                    // set to next element
                    cur = cur->next;
                }
            }
            break;
            
            case WAITING:
            {
                // TODO: Waiting on event
            }
            break;
            
            // fail state
            default:
            {
                // TODO: panic
            }
            break;
        }
	}
}


uint8_t scheduler_add_task(task_t *task)
{
    scheduler_node_t* ins =
            (scheduler_node_t*)malloc(sizeof(scheduler_node_t));
	
	if (ins != NULL)
	{
		// create new node
		ins->content = task;
		ins->next = NULL;
		
		// when no elements in list
		if (_root == NULL)
			_root = ins;
		
		else
		{
			scheduler_node_t *cur = _root;
			scheduler_node_t *pre = NULL;
			uint8_t is_inserted = 0;
			
			while (!is_inserted)
			{
				// insert before current position
				if (ins->content->priority > cur->content->priority)
				{
					// first position
					if (cur == _root)
					{
						ins->next = cur;
						_root = ins;
					}
					
					// within position
					else
					{
						ins->next = cur;
						pre->next = ins;
					}
					
					is_inserted = 1;
				}
				
				// set to next
				pre = cur;
				cur = cur->next;
			}
		}
        
        // enable scheduling
        scheduler_start_task(task);
        
        // task add success
        return 1;
	}
    
    // task add failed
    else
        return 0;
}
