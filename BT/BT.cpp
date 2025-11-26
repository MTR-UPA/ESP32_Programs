#include "BT.h"

static const char *TAG = "BT_SERIAL";

static BT* s_bt_instance = nullptr;

static void spp_callback_wrapper(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    if (s_bt_instance)
        s_bt_instance->spp_cb(event, param);
}


// the destructor frees up allocated memory
BT::~BT()
{
  _connected = false;
  s_bt_instance = nullptr;
}

/* Mantener el handle de la conexión SPP */

void BT::bt_set_spp_handle(uint32_t handle)
{
    _spp_conn_handle = handle;
}

/* Enviar datos binarios por SPP */
esp_err_t BT::bt_send_data(uint8_t *data, int len)
{
    if (_spp_conn_handle == 0)
    {
        ESP_LOGW(TAG, "No hay conexión SPP activa");
        return ESP_ERR_INVALID_STATE;
    }
    return esp_spp_write(_spp_conn_handle, len, data);
}

/* Enviar una cadena C terminada en '\0' */
esp_err_t BT::bt_send_string(const char *str)
{
    if (str == NULL) return ESP_ERR_INVALID_ARG;
    int len = (int)strlen(str);
    return bt_send_data((uint8_t *)str, len);
}

void BT::spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    
    switch (event)
    {
    case ESP_SPP_INIT_EVT:
        esp_bt_dev_set_device_name("robotito");
        esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0, "SPP_SERVER");
        ESP_LOGI(TAG, "Bluetooth listo");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        /* Guardar el handle de la conexión entrante */
        bt_set_spp_handle(param->srv_open.handle);
        ESP_LOGI(TAG, "SPP conexión abierta, handle=%u", (unsigned)_spp_conn_handle);
        /* Opcional: enviar saludo al abrir la conexión */
        bt_send_string("hola");
        _connected = true;
        break;
    case ESP_SPP_DATA_IND_EVT:
        /* Procesar datos recibidos por Bluetooth */
        spp_data_received(param->data_ind.data, param->data_ind.len);
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(TAG, "SPP conexión cerrada, handle=%u", (unsigned)_spp_conn_handle);
        bt_set_spp_handle(0);
        _connected = false;
        break;
    default:
        break;
    }
}

void BT::connectToBluetooth()
{
    esp_err_t ret;
    // Inicializar NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Inicializando Bluetooth...");

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    bt_cfg.mode = ESP_BT_MODE_CLASSIC_BT;

    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error init controlador: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error enable controlador: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error init bluedroid: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret != ESP_OK)
     {
        ESP_LOGE(TAG, "Error enable bluedroid: %s", esp_err_to_name(ret));
        return;
    }
    /* Registrar callback SPP y inicializar SPP */
    s_bt_instance = this;
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);

    ret = esp_spp_register_callback(spp_callback_wrapper);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error registering SPP callback: %s", esp_err_to_name(ret));
        return;
    }

    
    ret = esp_spp_init(ESP_SPP_MODE_CB);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error init SPP: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "✅ Bluetooth inicializado correctamente");
    ESP_LOGI(TAG, "Esperando conexión SPP para enviar mensajes...");

    /* Esperar hasta que spp_conn_handle sea distinto de 0 (conexión activa) */
    while (_spp_conn_handle == 0) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    
    ESP_LOGI(TAG, "Conexión SPP activa, handle=%u", (unsigned)_spp_conn_handle);
    _connected = true;
}

bool BT::connectedToBluetooth()
{
    return _connected;
}

/* Recibir datos por SPP */
void BT::spp_data_received(uint8_t *data, int len)
{
    if (data == NULL || len <= 0) return;
    
    //ESP_LOGI(TAG, "Datos recibidos: %d bytes", len);
       
    // Convertir a string para debugging
    char buffer[len + 1];
    memcpy(buffer, data, len);
    buffer[len] = '\0';
    //ESP_LOGI(TAG, "Contenido: %s", buffer);
    
    // Enviar datos a main a través de queue o callback
    if (data_callback) {
        data_callback(buffer, len);
    }
}