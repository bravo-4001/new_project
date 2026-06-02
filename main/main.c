#include "tasks.h"
#include "twai.h"
#include "main.h"
#include "ble_func.h"

off_on state;
ESPToDSPData_t transmit_data;
DSPToESPData_t receive_data;

void app_main(void)
{

    transmit_data.ChargingState = 3;
    transmit_data.MaximumPower =  150.0f*10.0f;
    transmit_data.TargetCurrent = 10.6f*10.0f;
    transmit_data.TurnOn = 1;
    transmit_data.TargetVoltage = 152.5*10;

    // This whole code is for the Led blinking 
    gpio_reset_pin(BLINK_LED);
    gpio_set_direction(BLINK_LED, GPIO_MODE_OUTPUT);

    xTaskCreate(toggle_task, "toggle_task", 2*1024, NULL, 5, &toggle_task_handle);
    xTaskCreate(resume_toggle_task, "resume_toggle_task", 2*1024, NULL, 5, &resume_toggle_task_handle);

    // TWAI CONFIGURATION DONE 
    twai_configuration();
    xTaskCreate(can_seq_trans_task, "can_seq_trans_task", 2*1024, NULL, 5, &can_seq_trans_task_handle);
    xTaskCreate(can_seq_receive_task, "can_seq_receive_task", 2*1024, NULL, 5, &can_seq_receive_task_handle);

    // BLE CONFIGURATION DONE
    esp_err_t ret = ble_init(); 
    if(ret != ESP_OK){
        ESP_LOGE("ble init", "BLE initialization failed with error: %d", ret);
    } else {
        ESP_LOGI("ble init", "BLE initialization successful");
    }
}
