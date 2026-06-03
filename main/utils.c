#include "utils.h"
#include "main.h"

esp_err_t write_to_ble(uint8_t *data, uint16_t *len)
{
    uint16_t idx = 0;
    uint16_t buffer = 0;
    uint32_t buffer32 = 0;

    [idx++] data= 0x01;      // Packet Type

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

    return ESP_OK;
}

esp_err_t read_from_ble(uint8_t *data, uint16_t* len)
{
    if (data == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (data[0] != 0x02)
    {
        return ESP_ERR_INVALID_RESPONSE;
    }

    uint16_t index = 1;
    uint16_t buffer = 0;
    uint8_t field_len = 0;

    // TurnOn
    field_len = data[index++];
    if (field_len != 1)
    {
        return ESP_ERR_INVALID_SIZE;
    }
    transmit_data.TurnOn = data[index++];

    // TargetVoltage
    field_len = data[index++];
    if (field_len != 2)
    {
        return ESP_ERR_INVALID_SIZE;
    }
    buffer = ((uint16_t)data[index] << 8) |
             ((uint16_t)data[index + 1]);
    index += 2;
    transmit_data.TargetVoltage = (float)buffer / 10.0f;

    // TargetCurrent
    field_len = data[index++];
    if (field_len != 2)
    {
        return ESP_ERR_INVALID_SIZE;
    }
    buffer = ((uint16_t)data[index] << 8) |
             ((uint16_t)data[index + 1]);
    index += 2;
    transmit_data.TargetCurrent = (float)buffer / 10.0f;

    // MaximumPower
    field_len = data[index++];
    if (field_len != 2)
    {
        return ESP_ERR_INVALID_SIZE;
    }
    buffer = ((uint16_t)data[index] << 8) |
             ((uint16_t)data[index + 1]);
    index += 2;
    transmit_data.MaximumPower = (float)buffer / 10.0f;

    // ChargingState
    field_len = data[index++];
    if (field_len != 1)
    {
        return ESP_ERR_INVALID_SIZE;
    }
    transmit_data.ChargingState = data[index++];

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

