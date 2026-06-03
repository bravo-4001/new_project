#ifndef BLE_FUNC_H
#define BLE_FUNC_H

// BLE function declarations and utilities go here.
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_gap.h"
#include "services/gap/ble_svc_gap.h"
#include "host/ble_gatt.h"
#include "services/gatt/ble_svc_gatt.h"

/* Defines */
#define DEVICE_NAME "30kWPCM_00002"
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define PCM_SVC_UUID                         \
    0x45, 0x23, 0xe1, 0xd1, 0x6f, 0x0a, 0x9e, 0xbc, \
        0xf1, 0x42, 0x3b, 0x4d, 0x01, 0x00, 0x00, 0x00

#define PCM_CHAR_UUID                         \
    0x45, 0x23, 0xe1, 0xd1, 0x6f, 0x0a, 0x9e, 0xbc, \
        0xf1, 0x42, 0x3b, 0x4d, 0x11, 0x00, 0x00, 0x00

#define PCM_SETTING_CHAR_UUID                 \
    0x45, 0x23, 0xe1, 0xd1, 0x6f, 0x0a, 0x9e, 0xbc, \
        0xf1, 0x42, 0x3b, 0x4d, 0x12, 0x00, 0x00, 0x00

#define BLE_GAP_APPEARANCE_GENERIC_TAG 0x0540

typedef enum {
    READ_CMD = 0x01,
    WRITE_CMD = 0x02
} cmd_type_t;

esp_err_t ble_init(void);
/* GAP */
int gap_init(void);
void adv_init(void);

/* GATT */
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
void gatt_svr_subscribe_cb(struct ble_gap_event *event);
int gatt_svc_init(void);

/* extern functions for gatt services */
// void send_indicate(uint8_t cmd,bool success_flag);
void send_indicate(uint8_t *data, uint16_t len);

#endif // BLE_FUNC_H
