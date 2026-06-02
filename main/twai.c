#include "twai.h"

void twai_configuration(void){
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(32, 33, TWAI_MODE_NORMAL);
    // twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(22, 21, TWAI_MODE_NO_ACK);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    //Install CAN driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        printf("Driver installed\n");
    } else {
        printf("Failed to install driver\n");
        return;
    }

    //Start CAN driver
    if (twai_start() == ESP_OK) {
        printf("Driver started\n");
    } else {
        printf("Failed to start driver\n");
        return;
    }
}

// Transmission is successful if the function returns ESP_OK
esp_err_t transmit_seq_1(uint8_t* payload){
    twai_message_t message_seq_1;

// ###################################################################
// #############The above structure has following fields##############
// ###################################################################
// typedef struct {
//     uint32_t identifier;
//     uint8_t data[8];
//     uint8_t data_length_code;
//     uint32_t flags;
// } can_message_t;

    message_seq_1.identifier = 0x02;
    message_seq_1.flags = TWAI_MSG_FLAG_NONE;
    message_seq_1.data_length_code  = 8;

    // Putting the payload, since the payload contains 8 bytes 
    for (int i=0; i<8; i++){
        message_seq_1.data[i] = payload[i];
    }

    // // For checking the error info
    // twai_status_info_t status;

    // twai_get_status_info(&status);

    // printf("State=%d TX_ERR=%ld RX_ERR=%ld TX_FAILED=%ld\n",
    //     status.state,
    //     status.tx_error_counter,
    //     status.rx_error_counter,
    //     status.tx_failed_count);

    esp_err_t err =  twai_transmit(&message_seq_1, pdMS_TO_TICKS(200));
    return err;
}

void receive_seq_1(uint32_t* data_received){
    // Code for reading data from through the can bus 

    twai_message_t message;

    //     typedef struct
    // {
    //     uint32_t identifier;
    //     uint8_t data_length_code;
    //     uint8_t data[8];
    //     uint32_t flags;
    // } twai_message_t;
   
    esp_err_t err = twai_receive(&message, pdMS_TO_TICKS(1000));
    if (err == ESP_OK){
        printf("message is successfully received\n");
    }

    for (int i=0; i<8; i++){
        data_received[i] = message.data[i];
    }
}
