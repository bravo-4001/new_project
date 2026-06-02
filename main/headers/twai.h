#ifndef TWAI_H
#define TWAI_H

#include "driver/gpio.h"
#include "driver/twai.h"
#include "esp_err.h"

// exporting the functions 
extern void twai_configuration(void);
extern esp_err_t transmit_seq_1(uint8_t* payload);
extern void receive_seq_1(uint32_t* data_received);

#endif 