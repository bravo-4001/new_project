#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include "esp_err.h"
extern esp_err_t write_to_ble(uint8_t *data, uint16_t *len);
// extern esp_err_t write_to_ble(uint8_t* data, uint16_t *len, void *struct_data);
extern esp_err_t read_from_ble(uint8_t *data, uint16_t len);
extern esp_err_t read_buck_settings(uint8_t *data, uint16_t *len);

#endif