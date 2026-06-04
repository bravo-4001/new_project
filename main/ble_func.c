#include "ble_func.h"

#include <errno.h>
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_store.h"
#include "services/gatt/ble_svc_gatt.h"
#include "nvs_flash.h"
#include "main.h"
#include "utils.h"

/*static variables for logging*/
static const char *TAG = "BLE_FUNC:";

// static DSPToESPData_t ble_pcm_data; // global variable to show pcm data in the pcm char
// static ESPToDSPData_t ble_pcm_setting_data; // global variable to show pcm setting data in the pcm setting char

inline static void format_addr(char *addr_str, uint8_t addr[]);
static void print_conn_desc(struct ble_gap_conn_desc *desc);
static void start_advertising(void);
static int gap_event_handler(struct ble_gap_event *event, void *arg);

static uint8_t own_addr_type;
static uint8_t addr_val[6] = {0};
static char ble_device_name[] = DEVICE_NAME;
uint16_t g_conn_handle = BLE_HS_CONN_HANDLE_NONE;

inline static void format_addr(char *addr_str, uint8_t addr[])
{
    sprintf(addr_str, "%02X:%02X:%02X:%02X:%02X:%02X",
            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

static void print_conn_desc(struct ble_gap_conn_desc *desc)
{
    char addr_str[18] = {0};
    ESP_LOGI(TAG, "connection handle: %d", desc->conn_handle);

    format_addr(addr_str, desc->our_id_addr.val);
    ESP_LOGI(TAG, "device id address: type=%d, value=%s", desc->our_id_addr.type, addr_str);

    format_addr(addr_str, desc->peer_id_addr.val);
    ESP_LOGI(TAG, "peer id address: type=%d, value=%s", desc->peer_id_addr.type, addr_str);

    ESP_LOGI(TAG,
             "conn_itvl=%d, conn_latency=%d, supervision_timeout=%d, "
             "encrypted=%d, authenticated=%d, bonded=%d\n",
             desc->conn_itvl, desc->conn_latency, desc->supervision_timeout,
             desc->sec_state.encrypted, desc->sec_state.authenticated,
             desc->sec_state.bonded);
}

static void start_advertising(void)
{
    int rc = 0;
    const char *name;
    struct ble_hs_adv_fields adv_fields = {0};
    struct ble_hs_adv_fields rsp_fields = {0};
    struct ble_gap_adv_params adv_params = {0};

    adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    name = ble_svc_gap_device_name();

    uint8_t name_len = (uint8_t)strlen(name);
    if (name_len + 5 > 31)
    {
        name_len = 26;
        adv_fields.name_is_complete = 0;
    }
    else
    {
        adv_fields.name_is_complete = 1;
    }
    adv_fields.name = (uint8_t *)name;
    adv_fields.name_len = name_len;

    rc = ble_gap_adv_set_fields(&adv_fields);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "failed to set adv data: %d", rc);
        return;
    }

    rsp_fields.name = (uint8_t *)ble_svc_gap_device_name();
    rsp_fields.name_len = (uint8_t)strlen(ble_svc_gap_device_name());
    rsp_fields.name_is_complete = 1;
    rsp_fields.tx_pwr_lvl_is_present = 1;
    rsp_fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    rsp_fields.appearance_is_present = 1;
    rsp_fields.appearance = BLE_GAP_APPEARANCE_GENERIC_TAG;

    rc = ble_gap_adv_rsp_set_fields(&rsp_fields);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "failed to set rsp data: %d", rc);
        return;
    }

    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    adv_params.itvl_min = BLE_GAP_ADV_ITVL_MS(100);
    adv_params.itvl_max = BLE_GAP_ADV_ITVL_MS(150);

    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, gap_event_handler, NULL);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "failed to start advertising: %d", rc);
        return;
    }
    ESP_LOGI(TAG, "advertising started!");
}

static int gap_event_handler(struct ble_gap_event *event, void *arg)
{
    int rc = 0;
    struct ble_gap_conn_desc desc;

    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(TAG, "connection %s; status=%d",
                 event->connect.status == 0 ? "established" : "failed",
                 event->connect.status);
        ESP_LOGI(TAG, "mtu update; conn_handle=%d cid=%d mtu=%d",
                 event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
        if (event->connect.status == 0)
        {
            g_conn_handle = event->connect.conn_handle;

            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            if (rc != 0)
            {
                ESP_LOGE(TAG, "conn find failed: %d", rc);
                return rc;
            }
            print_conn_desc(&desc);

            struct ble_gap_upd_params params = {
                .itvl_min = 24,
                .itvl_max = 40,
                .latency = 0,
                .supervision_timeout = 600,
            };
            rc = ble_gap_update_params(event->connect.conn_handle, &params);
                    ESP_LOGI(TAG, "mtu update; conn_handle=%d cid=%d mtu=%d",
                 event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
            if (rc != 0)
            {
                ESP_LOGE(TAG, "param update failed: %d", rc);
                return rc;
            }
        }
        else
        {
            start_advertising();
        }
        return rc;

    case BLE_GAP_EVENT_DISCONNECT:
        g_conn_handle = BLE_HS_CONN_HANDLE_NONE;

        ESP_LOGI(TAG, "disconnected; reason=%d", event->disconnect.reason);
        start_advertising();
        return rc;

    case BLE_GAP_EVENT_CONN_UPDATE:
        ESP_LOGI(TAG, "connection updated; status=%d", event->conn_update.status);
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        if (rc != 0)
        {
            ESP_LOGE(TAG, "conn find failed: %d", rc);
            return rc;
        }
        print_conn_desc(&desc);
        return rc;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "advertise complete; reason=%d", event->adv_complete.reason);
        start_advertising();
        return rc;

    case BLE_GAP_EVENT_NOTIFY_TX:
        if ((event->notify_tx.status != 0) &&
            (event->notify_tx.status != BLE_HS_EDONE))
        {
            ESP_LOGI(TAG,
                     "notify event; conn_handle=%d attr_handle=%d "
                     "status=%d is_indication=%d",
                     event->notify_tx.conn_handle, event->notify_tx.attr_handle,
                     event->notify_tx.status, event->notify_tx.indication);
        }
        return rc;

    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(TAG,
                 "subscribe event; conn_handle=%d attr_handle=%d "
                 "reason=%d prevn=%d curn=%d previ=%d curi=%d",
                 event->subscribe.conn_handle, event->subscribe.attr_handle,
                 event->subscribe.reason, event->subscribe.prev_notify,
                 event->subscribe.cur_notify, event->subscribe.prev_indicate,
                 event->subscribe.cur_indicate);
        gatt_svr_subscribe_cb(event);
        return rc;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "mtu update; conn_handle=%d cid=%d mtu=%d",
                 event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
        return rc;
    }
    return rc;
}

void adv_init(void)
{
    int rc = 0;
    char addr_str[18] = {0};

    rc = ble_hs_util_ensure_addr(0);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "no bt address!");
        return;
    }

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "infer addr type failed: %d", rc);
        return;
    }

    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "copy addr failed: %d", rc);
        return;
    }

    format_addr(addr_str, addr_val);
    ESP_LOGI(TAG, "device address: %s", addr_str);
    start_advertising();
}

int gap_init(void)
{
    int rc = 0;
    ble_svc_gap_init();
    rc = ble_svc_gap_device_name_set(ble_device_name);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "failed to set device name: %d", rc);
        return rc;
    }
    return rc;
}
/*=================gap server block end===================*/

/*=================gatt server block start===================*/
static uint16_t pcm_char_handle;
static uint16_t pcm_char_val_len;
static uint16_t pcm_setting_char_handle;
static const ble_uuid128_t pcm_svc_uuid = BLE_UUID128_INIT(PCM_SVC_UUID);
static const ble_uuid128_t pcm_char_uuid = BLE_UUID128_INIT(PCM_CHAR_UUID);
static const ble_uuid128_t pcm_setting_char_uuid = BLE_UUID128_INIT(PCM_SETTING_CHAR_UUID);

// forward declaration of the access handler
static int pcm_char_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg);
static int pcm_setting_char_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &pcm_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = &pcm_char_uuid.u,
                .access_cb = pcm_char_access,
                .val_handle = &pcm_char_handle,  
                .flags =  BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_INDICATE,
            },
            {
                .uuid = &pcm_setting_char_uuid.u,
                .access_cb = pcm_setting_char_access,
                .val_handle = &pcm_setting_char_handle,
                .flags =  BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_INDICATE,
            },
            {
                0, /* No more characteristics in this service. */
            }},
    },
    {
        0, /* No more services. */
    },
};

void gatt_svr_subscribe_cb(struct ble_gap_event *event)
{
    if (event->subscribe.conn_handle != BLE_HS_CONN_HANDLE_NONE)
        ESP_LOGI(TAG, "subscribe; conn_handle=%d attr_handle=%d",
                 event->subscribe.conn_handle, event->subscribe.attr_handle);
    else
        ESP_LOGI(TAG, "subscribe by nimble stack; attr_handle=%d",
                 event->subscribe.attr_handle);
}

int gatt_svc_init(void)
{
    int rc;
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0)
        return rc;

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0)
        return rc;

    return 0;
}

/*=================nimble host config & task==============*/
void ble_store_config_init(void);

/*
 * on_stack_reset:
 *   Called when NimBLE host resets BLE stack due to errors.
 *   Logs the reset reason for debugging.
 */
static void on_stack_reset(int reason)
{
    ESP_LOGI(TAG, "nimble stack reset, reset reason: %d", reason);
}

/*
 * on_stack_sync:
 *   Called when NimBLE host has synced with the BLE controller.
 *   Kicks off advertising so the device becomes discoverable.
 */
static void on_stack_sync(void)
{
    ESP_LOGI(TAG, "nimble stack synced, starting advertising...");
    adv_init();
}

static void nimble_host_config_init(void)
{
    /* Stack event callbacks */
    ble_hs_cfg.reset_cb = on_stack_reset;
    ble_hs_cfg.sync_cb = on_stack_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    // /* Store host configuration (bond/peer data in NVS) */
    // ble_store_config_init();
}

/*
 * nimble_host_task:
 *   FreeRTOS task that runs the NimBLE host stack.
 *   Blocks until nimble_port_stop() is called.
 */
static void nimble_host_task(void *param)
{
    ESP_LOGI(TAG, "nimble host task started!");

    /* Blocking call — runs the NimBLE event loop */
    nimble_port_run();

    /* Reached only after nimble_port_stop() */
    vTaskDelete(NULL);
}

// Full ble function init
esp_err_t ble_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK)
    {

        ESP_LOGE(TAG,
                 "NVS initialization failed: %s",
                 esp_err_to_name(err));
    }

    int rc = 0;
    esp_err_t ret = ESP_FAIL;
    ret = nimble_port_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "failed to initialise NimBLE stack, error: %d", ret);
        return ret;
    }
    ESP_LOGI(TAG, "NimBLE stack initialised");

#if CONFIG_BT_NIMBLE_GAP_SERVICE
    rc = gap_init();
    if (rc != 0)
    {
        ESP_LOGE(TAG, "failed to initialise GAP service, error: %d", rc);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "GAP service initialised");
#endif

    /*
     *  GATT server initialisation
     *    Registers DeviceConfig characteristic (READ | WRITE)
     *    with TLV serialise/deserialise support
     */
    rc = gatt_svc_init();
    if (rc != 0)
    {
        ESP_LOGE(TAG, "failed to initialise GATT server, error: %d", rc);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "GATT server initialised");
    nimble_host_config_init();
    ESP_LOGI(TAG, "NimBLE host configured with callbacks and store");

    xTaskCreate(nimble_host_task,
                "NimBLE Host",
                4 * 1024,
                NULL,
                5,
                NULL);
    return ret;
}

// charactersic access handler
static int pcm_char_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint8_t buf[256] = {0};
    uint16_t len = 0;
    esp_err_t ret;
    /* Handle write operation */
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR)
    {

        len = ctxt->om->om_len;
        if (len > 0)
        {
            os_mbuf_copydata(ctxt->om, 0, len, buf);
            // if (buf[0] == WRITE_CMD)
            // {
            //     uint8_t cmd = buf[0];
            //     ESP_LOGI(TAG, "Received write command for PCM characteristic: cmd=%d", cmd);
            //     ret = read_from_ble(buf, len);
            //     if (ret != ESP_OK)
            //     {
            //         ESP_LOGE(TAG, "Failed to write data for PCM characteristic: %d", ret);
            //                                 uint8_t indicate_data[3] = {0};
            //             indicate_data[0] = 0x03;
            //             indicate_data[1] = cmd;
            //             indicate_data[2] = 0x00; 
            //         send_indicate(indicate_data, sizeof(indicate_data));
            //         return BLE_ATT_ERR_UNLIKELY;
            //     }
            //     else
            //     {
            //         ESP_LOGI(TAG, "Data write for PCM characteristic: %.*s", len, buf);
            //         // Process the data read as needed
            //             uint8_t indicate_data[3] = {0};
            //             indicate_data[0] = 0x03;
            //             indicate_data[1] = cmd;
            //             indicate_data[2] = 0x01; // success flag
            //         send_indicate(indicate_data, sizeof(indicate_data));
            //     }
            // }
            // else if(buf[0] == READ_CMD)
            if(buf[0] == READ_CMD)
            {
                ESP_LOGI(TAG, "Received read command for PCM characteristic");
                // Handle the read command as needed
                        ret = write_to_ble(buf, &len);
                        uint8_t indicate_data[3] = {0};
                        uint8_t cmd = buf[0];
                        if (ret != ESP_OK)
                        {
                            ESP_LOGE(TAG, "Failed to read data for PCM characteristic: %d", ret);
                            indicate_data[0] = 0x03;
                            indicate_data[1] = cmd;
                            indicate_data[2] = 0x00; 
                            send_indicate(indicate_data, sizeof(indicate_data));
                            return BLE_ATT_ERR_UNLIKELY;
                        }
                        else
                        {
                            ESP_LOGI(TAG, "Read data for PCM characteristic: %.*s", len, buf);
                            if (len == 0)
                            {
                                indicate_data[2] = 0x00; // failure flag
                                send_indicate(indicate_data, sizeof(indicate_data));
                                ESP_LOGW(TAG, "No data to read for PCM characteristic");
                                return BLE_ATT_ERR_UNLIKELY;
                            }
                            else
                            {
                                send_indicate(buf, len);
                            }
                        }
                
            }

            ESP_LOGI(TAG, "Received data on PCM characteristic: %.*s", len, buf);
            // Process the received data as needed
        }
    }
    // else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR)
    // {

    //     ret = write_to_ble(buf, &len);

    //     if (ret != ESP_OK)
    //     {
    //         ESP_LOGE(TAG, "Failed to read data for PCM characteristic: %d", ret);
    //         return BLE_ATT_ERR_UNLIKELY;
    //     }
    //     else
    //     {
    //         ESP_LOGI(TAG, "Read data for PCM characteristic: %.*s", len, buf);
    //         if (len == 0)
    //         {
    //             ESP_LOGW(TAG, "No data to read for PCM characteristic");
    //             return BLE_ATT_ERR_UNLIKELY;
    //         }
    //         else
    //         {
    //             // Process the data to be sent as needed
    //             os_mbuf_append(ctxt->om, buf, len);
    //             // send_indicate(READ_CMD, true);
    //         }
    //     }

    //     // Handle read request if necessary
    //     ESP_LOGI(TAG, "Read request received on PCM characteristic");
    // }

    return 0;
}

static int pcm_setting_char_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg){
    uint8_t buf[256] = {0};
    uint16_t len = 0;
    uint8_t cmd = 0;
    esp_err_t ret;
    /* Handle write operation */
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR)
    {

        len = ctxt->om->om_len;
        if (len > 0)
        {
            os_mbuf_copydata(ctxt->om, 0, len, buf);

            
            if (buf[0] == WRITE_CMD)
            {
                uint8_t cmd = buf[0];
                // ESP_LOGI(TAG, "write command: cmd=%d", cmd);
                ret = read_from_ble(buf, len);
               
                ESP_LOGI(TAG,"RET value from read_from_ble: %d", ret);
                if (ret != ESP_OK)
                {
                    ESP_LOGE(TAG, "Failed to write data for PCM setting characteristic: %d", ret);
                                            uint8_t indicate_data[3] = {0};
                        indicate_data[0] = 0x03;
                        indicate_data[1] = cmd;
                        indicate_data[2] = 0x00; 
                    send_indicate_settings(indicate_data, sizeof(indicate_data));
                    return BLE_ATT_ERR_UNLIKELY;
                }
                else
                {
                    ESP_LOGI(TAG, "Data write for PCM setting characteristic: %.*s", len, buf);
                    // Process the data read as needed
                        uint8_t indicate_data[3] = {0};
                        indicate_data[0] = 0x03;
                        indicate_data[1] = cmd;
                        indicate_data[2] = 0x01; // success flag
                    send_indicate_settings(indicate_data, sizeof(indicate_data));
                }
            }
            else if(buf[0] == READ_CMD)
            {
                ESP_LOGI(TAG, "Received read command for PCM setting characteristic");
                // Handle the read command as needed
                        ret = read_buck_settings(buf, &len);
                        uint8_t indicate_data[3] = {0};
                        uint8_t cmd = buf[0];
                        ESP_LOGI(TAG, "Processing read command for PCM setting characteristic: cmd=%d", cmd);
                        // ESP_LOGI(TAG,"BUFFER VALUE from write_settings_to_ble: %.*s", len, buf);
                        // send_indicate_settings(buf, len);

                        if (ret != ESP_OK)
                        {
                            ESP_LOGE(TAG, "Failed to read data for PCM setting characteristic: %d", ret);
                            indicate_data[0] = 0x03;
                            indicate_data[1] = cmd;
                            indicate_data[2] = 0x00; 
                            send_indicate_settings(indicate_data, sizeof(indicate_data));
                            return BLE_ATT_ERR_UNLIKELY;
                        }
                        else
                        {

                            if (len == 0)
                            {
                                indicate_data[2] = 0x00; // failure flag
                                send_indicate_settings(indicate_data, sizeof(indicate_data));
                                ESP_LOGW(TAG, "No data to read for PCM setting characteristic");
                                return BLE_ATT_ERR_UNLIKELY;
                            }
                            else
                            {

                                send_indicate_settings(buf, len);
                              
                            }
                        }
                
            }

            ESP_LOGI(TAG, "Received data on PCM setting characteristic: %.*s", len, buf);
            // Process the received data as needed
        }
    }
    // else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR)
    // {

    //     ret = write_to_ble(buf, &len);

    //     if (ret != ESP_OK)
    //     {
    //         ESP_LOGE(TAG, "Failed to read data for PCM characteristic: %d", ret);
    //         return BLE_ATT_ERR_UNLIKELY;
    //     }
    //     else
    //     {
    //         ESP_LOGI(TAG, "Read data for PCM characteristic: %.*s", len, buf);
    //         if (len == 0)
    //         {
    //             ESP_LOGW(TAG, "No data to read for PCM characteristic");
    //             return BLE_ATT_ERR_UNLIKELY;
    //         }
    //         else
    //         {
    //             // Process the data to be sent as needed
    //             os_mbuf_append(ctxt->om, buf, len);
    //             // send_indicate(READ_CMD, true);
    //         }
    //     }

    //     // Handle read request if necessary
    //     ESP_LOGI(TAG, "Read request received on PCM characteristic");
    // }

    return 0;
                    
}


/*==================gatt server block end===================*/

void send_indicate(uint8_t *data, uint16_t len)
{
    if (g_conn_handle == BLE_HS_CONN_HANDLE_NONE)
    {
        ESP_LOGW(TAG, "No active connection to send indication");
        return;
    }

    if(len > 180) // for limiting the data length to avoid fragmentation issues
    {
        ESP_LOGW(TAG, "Data length exceeds 180 bytes, truncating for indication");
        len = 180;
    }
    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, len);
    if (om == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate mbuf");
        return;
    }

    int rc = ble_gatts_indicate_custom(g_conn_handle, pcm_char_handle, om);

    if (rc != 0)
    {
        ESP_LOGE(TAG, "Failed to send indication: %d", rc);
    }
    else
    {
        ESP_LOGI(TAG, "Indication sent successfully: ");
    }
}

void send_indicate_settings(uint8_t *data, uint16_t len)
{
    if (g_conn_handle == BLE_HS_CONN_HANDLE_NONE)
    {
        ESP_LOGW(TAG, "No active connection to send indication");
        return;
    }

    if(len > 180) // for limiting the data length to avoid fragmentation issues
    {
        ESP_LOGW(TAG, "Data length exceeds 180 bytes, truncating for indication");
        len = 180;
    }
    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, len);
    if (om == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate mbuf");
        return;
    }

    int rc = ble_gatts_indicate_custom(g_conn_handle, pcm_setting_char_handle, om);

    if (rc != 0)
    {
        ESP_LOGE(TAG, "Failed to send indication: %d", rc);
    }
    else
    {
        ESP_LOGI(TAG, "Indication sent successfully: ");
    }
}


// // Additional utility functions for data synchronization can be implemented here
// esp_err_t fuction_to_sync_dsptoesp_data(DSPToESPData_t *data){
//     // Implement the logic to sync data from DSP to ESP
//     // This function can be called when new data is received from the DSP
//     // and needs to be updated in the global variable for BLE indication

//     if (data == NULL)
//     {
//         ESP_LOGE(TAG, "Invalid input: data pointer is NULL");
//         return ESP_ERR_INVALID_ARG;
//     }

//     // Update the global variable with the new data
//     ble_pcm_data = *data;

//     ESP_LOGI(TAG, "Synchronized DSP to ESP data successfully");
//     return ESP_OK;
// }

// esp_err_t fuction_to_sync_esptodsp_data(ESPToDSPData_t *data){
//     // Implement the logic to sync data from ESP to DSP
//     // This function can be called when new data is received from the BLE write
//     // and needs to be updated in the global variable for processing by the DSP

//     if (data == NULL)
//     {
//         ESP_LOGE(TAG, "Invalid input: data pointer is NULL");
//         return ESP_ERR_INVALID_ARG;
//     }

//     // Update the global variable with the new data
//     ble_pcm_setting_data = *data;

//     ESP_LOGI(TAG, "Synchronized ESP to DSP data successfully");
//     return ESP_OK;
// }
