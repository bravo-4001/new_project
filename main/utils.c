#include "utils.h"
#include "main.h"
#include <errno.h>
#include "esp_log.h"

// esp_err_t write_to_ble(uint8_t *data, uint16_t *len, void *struct_data)
esp_err_t write_to_ble(uint8_t *data, uint16_t *len)
{
    if (data == NULL || len == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
            
    uint16_t idx = 0;
    uint16_t buffer = 0;
    uint32_t buffer32 = 0;

    data[idx++] = 0x01;      // Packet Type
    
    // if(struct_data == NULL)
    // {
    // Vin1
    data[idx++] = 2;
    buffer = (uint16_t)(receive_data.Vin1 * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // Vin2
    data[idx++] = 2;
    buffer = (uint16_t)(receive_data.Vin2 * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // Vout1
    data[idx++] = 2;
    buffer = (uint16_t)(receive_data.Vout1 * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // Vout2
    data[idx++] = 2;
    buffer = (uint16_t)(receive_data.Vout2 * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // Iout1
    data[idx++] = 2;
    buffer = (uint16_t)(receive_data.Iout1 * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // Iout2
    data[idx++] = 2;
    buffer = (uint16_t)(receive_data.Iout2 * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // Fault Code
    data[idx++] = 1;
    data[idx++] = receive_data.FaultCode;

    // ISR State
    data[idx++] = 1;
    data[idx++] = receive_data.IsrState;

    // Vout Final
    data[idx++] = 3;
    buffer32 = (uint32_t)(receive_data.VoutFinal * 10);
    data[idx++] = (buffer32 >> 16) & 0xFF;
    data[idx++] = (buffer32 >> 8) & 0xFF;
    data[idx++] = buffer32 & 0xFF;

    // Fan Voltage
    data[idx++] = 2;
    buffer = (uint16_t)(receive_data.FanVoltage * 100);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // Buck Mode 1
    data[idx++] = 1;
    data[idx++] = receive_data.CcCvMode_Buck1;

    // Buck Mode 2
    data[idx++] = 1;
    data[idx++] = receive_data.CcCvMode_Buck2;

    // HeatSinkTemp1
    data[idx++] = 2;
    buffer = (uint16_t)((receive_data.HeatSinkTemp1 + 500) * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // HeatSinkTemp2
    data[idx++] = 2;
    buffer = (uint16_t)((receive_data.HeatSinkTemp2 + 500) * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // HeatSinkTemp3
    data[idx++] = 2;
    buffer = (uint16_t)((receive_data.HeatSinkTemp3 + 500) * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // HeatSinkTemp4
    data[idx++] = 2;
    buffer = (uint16_t)((receive_data.HeatSinkTemp4 + 500) * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // InductorTemp1
    data[idx++] = 2;
    buffer = (uint16_t)((receive_data.InductorTemp1 + 500) * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // InductorTemp2
    data[idx++] = 2;
    buffer = (uint16_t)((receive_data.InductorTemp2 + 500) * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    *len = idx;
//     }
//     else{
//     DSPToESPData_t *settings_data = (DSPToESPData_t *)struct_data;
//     // Update the settings data with the received values
//     data[idx++] = 2;
//     buffer = (uint16_t)(settings_data->Vin1 * 10);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;

//     data[idx++] = 2;
//     buffer = (uint16_t)(settings_data->Vin2 * 10);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;

//     data[idx++] = 2;
//     buffer = (uint16_t)(settings_data->Vout1 * 10);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;   

//     data[idx++] = 2;
//     buffer = (uint16_t)(settings_data->Vout2 * 10);
//     data[idx++] = (buffer >> 8) & 0xFF; 
//     data[idx++] = buffer & 0xFF;

//     data[idx++] = 2;
//     buffer = (uint16_t)(settings_data->Iout1 * 10);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;

//     data[idx++] = 2;
//     buffer = (uint16_t)(settings_data->Iout2 * 10);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;

//     data[idx++] = 1;
//     data[idx++] = settings_data->FaultCode;
//     data[idx++] = 1;
//     data[idx++] = settings_data->IsrState;
//     data[idx++] = 3;
//     buffer32 = (uint32_t)(settings_data->VoutFinal * 10);
//     data[idx++] = (buffer32 >> 16) & 0xFF;
//     data[idx++] = (buffer32 >> 8) & 0xFF;
//     data[idx++] = buffer32 & 0xFF;

//     data[idx++] = 2;
//     buffer = (uint16_t)(settings_data->FanVoltage * 100);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;

//     data[idx++] = 1;
//     data[idx++] = settings_data->CcCvMode_Buck1;
//     data[idx++] = 1;
//     data[idx++] = settings_data->CcCvMode_Buck2;
//     data[idx++] = 2;
//     buffer = (uint16_t)((settings_data->HeatSinkTemp1 + 500) * 10);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;
//     data[idx++] = 2;
//     buffer = (uint16_t)((settings_data->HeatSinkTemp2 + 500) * 10);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;
//     data[idx++] = 2;
//     buffer = (uint16_t)((settings_data->HeatSinkTemp3 + 500)    * 10);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;
//     data[idx++] = 2;
//     buffer = (uint16_t)((settings_data->HeatSinkTemp4 + 500) * 10);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;
//     data[idx++] = 2;
//     buffer = (uint16_t)((settings_data->InductorTemp1 + 500) * 10);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;
//     data[idx++] = 2;
//     buffer = (uint16_t)((settings_data->InductorTemp2 + 500) * 10);
//     data[idx++] = (buffer >> 8) & 0xFF;
//     data[idx++] = buffer & 0xFF;
    
//     *len = idx;
//      ESP_LOGI("UTILS", "Data written to BLE for PCM setting characteristic: Vin1=%.2f, Vin2=%.2f, Vout1=%.2f, Vout2=%.2f, Iout1=%.2f, Iout2=%.2f, FaultCode=%d, IsrState=%d, VoutFinal=%.2f, FanVoltage=%.2f, CcCvMode_Buck1=%d, CcCvMode_Buck2=%d, HeatSinkTemp1=%.2f, HeatSinkTemp2=%.2f, HeatSinkTemp3=%.2f, HeatSinkTemp4=%.2f, InductorTemp1=%.2f, InductorTemp2=%.2f",
//              settings_data->Vin1, settings_data->Vin2, settings_data->Vout1,
//              settings_data->Vout2, settings_data->Iout1, settings_data->Iout2,
//              settings_data->FaultCode, settings_data->IsrState, settings_data->VoutFinal,
//              settings_data->FanVoltage, settings_data->CcCvMode_Buck1,
//              settings_data->CcCvMode_Buck2, settings_data->HeatSinkTemp1,
//              settings_data->HeatSinkTemp2, settings_data->HeatSinkTemp3,
//              settings_data->HeatSinkTemp4, settings_data->InductorTemp1,
//              settings_data->InductorTemp2);

// }

    return ESP_OK;
}

esp_err_t read_from_ble(uint8_t *data, uint16_t len)
{
    if (data == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    printf("\n");
    if (data[0] != 0x02)
    {
        return ESP_ERR_INVALID_RESPONSE;
    }

    uint16_t index = 1;
    uint16_t buffer = 0;
    uint8_t field_len = 0;

    // TurnOn
    field_len = data[index++];
    // if (field_len != 1)
    // {
    //     return ESP_ERR_INVALID_SIZE;
    // }
    transmit_data.TurnOn = data[index++];

    // TargetVoltage
    field_len = data[index++];
    // if (field_len != 2)
    // {
    //     return ESP_ERR_INVALID_SIZE;
    // }
    buffer = ((uint16_t)data[index] << 8) |
             ((uint16_t)data[index + 1]);
    index += 2;
    transmit_data.TargetVoltage = (float)buffer / 10.0f;

    // TargetCurrent
    field_len = data[index++];
    // if (field_len != 2)
    // {
    //     return ESP_ERR_INVALID_SIZE;
    // }
    buffer = ((uint16_t)data[index] << 8) |
             ((uint16_t)data[index + 1]);
    index += 2;
    transmit_data.TargetCurrent = (float)buffer / 10.0f;

    // MaximumPower
    field_len = data[index++];
    // if (field_len != 2)
    // {
    //     return ESP_ERR_INVALID_SIZE;
    // }
    buffer = ((uint16_t)data[index] << 8) |
             ((uint16_t)data[index + 1]);
    index += 2;
    transmit_data.MaximumPower = (float)buffer / 10.0f;

    // ChargingState
    field_len = data[index++];
    // if (field_len != 1)
    // {
    //     return ESP_ERR_INVALID_SIZE;
    // }
    transmit_data.ChargingState = data[index++];

    printf("Data read from BLE:\n");
    printf("TurnOn           = %d\n", transmit_data.TurnOn);
    printf("TargetVoltage    = %.2f\n", transmit_data.TargetVoltage);
    printf("TargetCurrent    = %.2f\n", transmit_data.TargetCurrent);
    printf("MaximumPower     = %.2f\n", transmit_data.MaximumPower);
    printf("ChargingState    = %d\n", transmit_data.ChargingState);
   
    return ESP_OK;
}


esp_err_t write_settings_to_ble(uint8_t *data, uint16_t *len)
{
    uint16_t idx = 0;
    uint16_t buffer = 0;

    data[idx++] = 0x01;      // Packet Type

    // TurnOn
    data[idx++] = 1;
    data[idx++] = transmit_data.TurnOn;

    // TargetVoltage
    data[idx++] = 2;
    buffer = (uint16_t)(transmit_data.TargetVoltage * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // TargetCurrent
    data[idx++] = 2;
    buffer = (uint16_t)(transmit_data.TargetCurrent * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // MaximumPower
    data[idx++] = 2;
    buffer = (uint16_t)(transmit_data.MaximumPower * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // ChargingState
    data[idx++] = 1;
    data[idx++] = transmit_data.ChargingState;

    *len = idx;

    ESP_LOGI("UTILS", "Data written to BLE for PCM setting characteristic: TurnOn=%d, TargetVoltage=%.2f, TargetCurrent=%.2f, MaximumPower=%.2f, ChargingState=%d",
             transmit_data.TurnOn, transmit_data.TargetVoltage, transmit_data.TargetCurrent,
             transmit_data.MaximumPower, transmit_data.ChargingState);
    return ESP_OK;
}

esp_err_t read_buck_settings(uint8_t *data, uint16_t* len){

    if (data == NULL || len == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    // Writing the code to make the application read the data to be transmitted 
    uint16_t idx = 0;
    uint16_t buffer = 0;
    uint32_t buffer32 = 0;

    data[idx++]= 0x01;      // Packet Type

    // For the turn on 
    data[idx++] = 1;
    buffer = (uint16_t)(transmit_data.TurnOn);
    data[idx++] = buffer & 0xFF;

    // For the target voltage
    data[idx++] = 3;
    buffer32 = (uint32_t)(transmit_data.TargetVoltage * 10);
    data[idx++] = (buffer32 >> 16) & 0xFF;
    data[idx++] = (buffer32 >> 8) & 0xFF;
    data[idx++] = buffer32 & 0xFF;

    // For the target current 
    data[idx++] = 3;
    buffer32 = (uint32_t)(transmit_data.TargetCurrent * 10);
    data[idx++] = (buffer32 >> 16) & 0xFF;
    data[idx++] = (buffer32 >> 8) & 0xFF;
    data[idx++] = buffer32 & 0xFF;

    // For the maximum power 
    data[idx++] = 2;
    buffer = (uint16_t)(transmit_data.MaximumPower * 10);
    data[idx++] = (buffer >> 8) & 0xFF;
    data[idx++] = buffer & 0xFF;

    // For the charging state 
    data[idx++] = 1;
    data[idx++] = transmit_data.ChargingState;

    *len = idx;

    return ESP_OK;
}