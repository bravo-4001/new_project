#include "tasks.h"
#include "freertos/idf_additions.h"
#include "twai.h"
#include <stdint.h>
#include "main.h"

TaskHandle_t toggle_task_handle;
TaskHandle_t resume_toggle_task_handle;  
TaskHandle_t can_seq_trans_task_handle;  
TaskHandle_t can_seq_receive_task_handle;  

void toggle_task(void* params){
    bool led_on = false;
    int count = 0;
    while (1){
        led_on = !led_on;
        gpio_set_level(BLINK_LED, led_on);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if ((count++) >= 5){
            vTaskSuspend(toggle_task_handle);
            vTaskResume(resume_toggle_task_handle);
            count = 0;
        }
    }
}

void resume_toggle_task(void* params){
    while (1){
        vTaskDelay(pdMS_TO_TICKS(12000));
        vTaskResume(toggle_task_handle);
        vTaskSuspend(resume_toggle_task_handle);
    }
}

void can_seq_trans_task(void* params){
    uint8_t payload[8];
    uint8_t seq_to_be_sent = 1;

    while (1){
        switch (seq_to_be_sent){
            case 1: { // first seq
                payload[0] = 1;
                payload[1] = transmit_data.TurnOn;

                uint32_t buffer = transmit_data.TargetVoltage;
                payload[2] = buffer&(0xff);
                buffer >>= 8;
                payload[3] = buffer&(0xff);
                buffer >>= 8;
                payload[4] = buffer&(0xff);

                buffer = transmit_data.TargetCurrent;
                payload[5] = buffer&(0xff);
                buffer >>= 8;
                payload[6] = buffer&(0xff);
                buffer >>= 8;
                payload[7] = buffer&(0xff);

                seq_to_be_sent++;
                break;
            } 
            case 2: { // second seq
                payload[0] = 2;

                uint32_t buffer = transmit_data.MaximumPower;
                payload[1] = buffer&(0xff);
                buffer >>= 8;
                payload[2] = buffer&(0xff);

                buffer = transmit_data.ChargingState;
                payload[3] = buffer;
                seq_to_be_sent = 1;
                break;
            }
        }
        esp_err_t err = transmit_seq_1(payload);
        printf("Transmit result = %s (%d)\n", esp_err_to_name(err), err);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void can_seq_receive_task(void* params){
    uint32_t data_received[8];

    while (1){
        receive_seq_1(data_received);

        uint8_t seq_num = data_received[0];
        uint32_t buffer = 0;

        switch (seq_num){
            case 1: {
                // For vin 1
                buffer = data_received[1];
                buffer |= (data_received[2]<<8);
                receive_data.Vin1 = (float)buffer; 
                receive_data.Vin1 /= 10;
                receive_data.Vin1 -= 0;

                // For vin 2
                buffer = data_received[3];
                buffer |= (data_received[4]<<8);
                receive_data.Vin2 = (float)buffer; 
                receive_data.Vin2 /= 10;
                receive_data.Vin2 -= 0; 
                
                // For vout 1
                buffer = data_received[5];
                buffer |= (data_received[6]<<8);
                receive_data.Vout1 = (float)buffer; 
                receive_data.Vout1 /= 10;
                receive_data.Vout1 -= 0;        
                
                // For the fault code 
                receive_data.FaultCode = data_received[7];
                break;
            }
            case 2: {
                // For vout 2
                buffer = data_received[1];
                buffer |= (data_received[2]<<8);
                receive_data.Vout2 = (float)buffer; 
                receive_data.Vout2 /= 10;
                receive_data.Vout2 -= 0;

                // For iout 1
                buffer = data_received[3];
                buffer |= (data_received[4]<<8);
                receive_data.Iout1 = (float)buffer; 
                receive_data.Iout1 /= 10;
                receive_data.Iout1 -= 0; 
                
                // For iout 2
                buffer = data_received[5];
                buffer |= (data_received[6]<<8);
                receive_data.Iout2 = (float)buffer; 
                receive_data.Iout2 /= 10;
                receive_data.Iout2 -= 0;        
                
                // For the isr state 
                receive_data.IsrState = data_received[7];
                break;
            }
            case 3: {
                // For vout final
                buffer = data_received[1];
                buffer |= (data_received[2]<<8);
                buffer |= (data_received[3]<<16);
                receive_data.VoutFinal = (float)buffer; 
                receive_data.VoutFinal /= 10;
                receive_data.VoutFinal -= 0;

                // For fan voltage
                buffer = data_received[4];
                buffer |= (data_received[5]<<8);
                receive_data.FanVoltage = (float)buffer; 
                receive_data.FanVoltage /= 100;
                receive_data.FanVoltage -= 0; 
                
                // For cc-cv buck 1
                receive_data.CcCvMode_Buck1 = data_received[6];      
                
                // For cc-cv buck 2
                receive_data.CcCvMode_Buck2 = data_received[7];      
                break;
            }
            case 4: {
                // For heat sink temperature 1
                buffer = data_received[1];
                buffer |= (data_received[2]<<8);
                receive_data.HeatSinkTemp1 = (float)buffer; 
                receive_data.HeatSinkTemp1 /= 10;
                receive_data.HeatSinkTemp1 -= 500;

                // For heat sink temperature 2
                buffer = data_received[3];
                buffer |= (data_received[4]<<8);
                receive_data.HeatSinkTemp2 = (float)buffer; 
                receive_data.HeatSinkTemp2 /= 10;
                receive_data.HeatSinkTemp2 -= 500;

                // For heat sink temperature 3
                buffer = data_received[5];
                buffer |= (data_received[6]<<8);
                receive_data.HeatSinkTemp3 = (float)buffer; 
                receive_data.HeatSinkTemp3 /= 10;
                receive_data.HeatSinkTemp3 -= 500;
                    
                break;
            }
            case 5: {
                // For heat sink temperature 4
                buffer = data_received[1];
                buffer |= (data_received[2]<<8);
                receive_data.HeatSinkTemp4 = (float)buffer; 
                receive_data.HeatSinkTemp4 /= 10;
                receive_data.HeatSinkTemp4 -= 500;

                // For inductor temperature 1
                buffer = data_received[3];
                buffer |= (data_received[4]<<8);
                receive_data.InductorTemp1 = (float)buffer; 
                receive_data.InductorTemp1 /= 10;
                receive_data.InductorTemp1 -= 500;

                // For inductor temperature 2
                buffer = data_received[5];
                buffer |= (data_received[6]<<8);
                receive_data.InductorTemp2 = (float)buffer; 
                receive_data.InductorTemp2 /= 10;
                receive_data.InductorTemp2 -= 500;
                    
                break;
            }
        }
        printf("Vin1            = %.2f\n", receive_data.Vin1);
        printf("Vin2            = %.2f\n", receive_data.Vin2);
        printf("Vout1           = %.2f\n", receive_data.Vout1);
        printf("FaultCode       = %u\n", receive_data.FaultCode);

        printf("Vout2           = %.2f\n", receive_data.Vout2);
        printf("Iout1           = %.2f\n", receive_data.Iout1);
        printf("Iout2           = %.2f\n", receive_data.Iout2);
        printf("IsrState        = %u\n", receive_data.IsrState);

        printf("VoutFinal       = %.2f\n", receive_data.VoutFinal);
        printf("FanVoltage      = %.2f\n", receive_data.FanVoltage);
        printf("CcCvMode_Buck1  = %u\n", receive_data.CcCvMode_Buck1);
        printf("CcCvMode_Buck2  = %u\n", receive_data.CcCvMode_Buck2);

        printf("HeatSinkTemp1   = %.2f\n", receive_data.HeatSinkTemp1);
        printf("HeatSinkTemp2   = %.2f\n", receive_data.HeatSinkTemp2);
        printf("HeatSinkTemp3   = %.2f\n", receive_data.HeatSinkTemp3);

        printf("HeatSinkTemp4   = %.2f\n", receive_data.HeatSinkTemp4);
        printf("InductorTemp1   = %.2f\n", receive_data.InductorTemp1);
        printf("InductorTemp2   = %.2f\n", receive_data.InductorTemp2);
    }
}