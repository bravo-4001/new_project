#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <stdint.h>

// structures definitons for the can transmit messages 
typedef struct
{
    uint8_t TurnOn;

    float TargetVoltage;
    float TargetCurrent;

    float MaximumPower;

    uint8_t ChargingState;

}ESPToDSPData_t;

typedef struct
{
    float Vin1;
    float Vin2;
    float Vout1;
    uint8_t FaultCode;

    float Vout2;
    float Iout1;
    float Iout2;
    uint8_t IsrState;
    
    float VoutFinal;
    float FanVoltage;
    uint8_t CcCvMode_Buck1;      // 0=CC 1=CV
    uint8_t CcCvMode_Buck2;

    float HeatSinkTemp1;
    float HeatSinkTemp2;
    float HeatSinkTemp3;

    float HeatSinkTemp4;
    float InductorTemp1;
    float InductorTemp2;


}DSPToESPData_t;

typedef enum{
    turn_on,
    turn_off
}off_on;

extern off_on state;
extern ESPToDSPData_t transmit_data;
extern DSPToESPData_t receive_data;

extern esp_err_t write_to_ble(uint8_t* data, uint16_t *len);
extern esp_err_t read_from_ble(uint8_t* data,  uint16_t len);

#endif