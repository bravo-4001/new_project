#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include "freertos//FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BLINK_LED 2


// extern TaskHandle_t toggle_task_handle;
// extern TaskHandle_t resume_toggle_task_handle;
extern TaskHandle_t can_seq_trans_task_handle; 
extern TaskHandle_t can_seq_receive_task_handle;

// extern void toggle_task(void* params);
// extern void resume_toggle_task(void* params);
extern void can_seq_trans_task(void* params);
extern void can_seq_receive_task(void* params);
#endif