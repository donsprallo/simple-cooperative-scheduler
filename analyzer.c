#include "analyzer.h"


// scheduler measure enable
uint8_t _measure_enabled = 0;

// analyzer task
task_t* _analyze_task = NULL;

// root element of measured tasks
static task_measure_t* _root = NULL;

// measured task counter
uint8_t _count = 0;


uint8_t analyzer_enable(void)
{
    // just when task is not enabled
    if (!_measure_enabled)
    {
        // add _analyze_handler to scheduler
        _analyze_task = scheduler_create_task(
                SCHEDULER_MEASURE_PRIORITY,
                SCHEDULER_MEASURE_INTERVAL,
                _analyze_handler, NULL);

        // just when task can malloc
        if (_analyze_task)
        {
            if (scheduler_add_task(_analyze_task))
            {
                // disable timer
               T2CONbits.TON = 0;
               T3CONbits.TON = 0;
               // enable 32bit timer mode
               T2CONbits.T32 = 1;
               // select internal clock
               T2CONbits.TCS = 0;
               // disable gated timer mode
               T2CONbits.TGATE = 0;
               // select prescaler 1:1
               T2CONbits.TCKPS = 0b00;

               // clear timer register
               TMR3 = 0;
               TMR2 = 0;

               // enable success
               _measure_enabled = 1;

               return 1;
            }

            // when task add fail
            else
                return 0;
        }

        // when task creation fail
        else
            return 0;
    }
    
    // when measure currently enabled
    else
        return 1;
}


void analyzer_disable(void)
{
    _measure_enabled = 0;
    
    if (_analyze_task != NULL)
        scheduler_remove_task(_analyze_task);
}


task_measure_t* analyzer_measure_task(task_t* task, uint8_t id)
{
    task_measure_t* item = 
            (task_measure_t*)malloc(sizeof(task_measure_t));
    
    // memory success
    if (item != NULL)
    {
        // init measure type
        item->id = id;
        item->task = task;
        item->measure = 0;
        item->next = NULL;
        
        // wrap measure data
        task->measure_callback = _analyzer_measure;
        task->measure_wrapper  = item;
        
        // add to list
        if (_root == NULL)
            _root = item;
        
        else
        {
            task_measure_t* cur = _root;
            
            // navigate to end of list
            while (cur->next != NULL)
                cur = cur->next;
            
            // append item to end of list
            cur->next = item;
        }
        
        _count++;

        return item;
    }
    
    // memory fail
    else
        return NULL;
}


// mesaure function
void _analyzer_measure(void* wrapper, uint8_t state)
{
    static uint32_t measure_start = 0;
    static uint32_t measure_end = 0;
    
    task_measure_t* item = (task_measure_t*)wrapper;
    
    switch (state)
    {
        // measure start
        case 0:
        {
            measure_start = _read_measure_timer();
            T2CONbits.TON = 1;
        }
            break;

        // measure end
            case 1:
        {
            T2CONbits.TON = 0;
            measure_end = _read_measure_timer();
            item->measure += measure_end - measure_start;
        }
        break;
        
        // measure fail
        default:
        {
            T2CONbits.TON = 0;
        }
            break;
    }
}


void split(uint32_t val, char* buffer, int n)
{
    int i;
    
    for (i = 0; i < n; i++)
    {
        *buffer = (char)val;
        val = val >> 8;
        buffer++;
    }
}


void _analyze_handler(task_t* task, void* param)
{
    char buffer[4];
    task_measure_t* cur = _root;    
    
    // get measured tasks total time
    uint32_t measure = _read_measure_timer();
    _reset_measure_timer();
    
    // send header
    split(4 + (_count * 5), buffer, 4);
    uart_send_bytes(buffer, 4);
    
    // send informations
    split(measure, buffer, 4);
    uart_send_bytes(buffer, 4);
    
    // reset all task measure values
    while (cur)
    {
        split(cur->measure, buffer, 4);
        uart_send_bytes((char*)&cur->id, 1);
        uart_send_bytes(buffer, 4);
        
        cur->measure = 0;
        cur = cur->next;
    }
    
    uart_flush();
}