#include <string.h>
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "esp_gap_bt_api.h"
#include "esp_log.h"

typedef void (*data_callback_t)(const char* data, int len);

class BT
{
public:
    BT() = default;
    ~BT();
    void bt_set_spp_handle(uint32_t handle);
    esp_err_t bt_send_data(uint8_t *data, int len);
    esp_err_t bt_send_string(const char *str);
    void spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
    void connectToBluetooth();
    bool connectedToBluetooth();
    void spp_data_received(uint8_t *data, int len);

     // Registrar callback
    void set_data_callback(data_callback_t callback) {
        data_callback = callback;
    }
    
private:
    uint32_t _spp_conn_handle = 0;
    bool _connected = false;
    data_callback_t data_callback = nullptr;
};
