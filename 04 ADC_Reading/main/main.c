#include <stdio.h> // Incluye la biblioteca estándar de entrada/salida
#include "freertos/FreeRTOS.h" // Incluye la biblioteca de FreeRTOS
#include "esp_adc/adc_oneshot.h" // Incluye el controlador ADC en modo de una sola toma
#include "esp_log.h"

static const char *ESP = "Mi ESP";
static const char *ADC = "Soy el ADC";

int adc_value = 0; // Variable para almacenar el valor leído del ADC
adc_oneshot_unit_handle_t adc1_handle; // Manejador para la unidad ADC1
int adc_raw = 0; // Variable para almacenar el valor crudo del ADC

void configuracion(void){
    
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1, // Identificador de la unidad ADC1
        .ulp_mode = ADC_ULP_MODE_DISABLE, // Deshabilita el modo ULP
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle); // Inicializa el ADC

    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12, // Atenuación de 12dB
        .bitwidth = ADC_BITWIDTH_12, // Ancho de datos de 12 bits
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &channel_config); // Configura el canal 0 del ADC1
    ESP_LOGE(ESP, "Ya termine la configuración \n");

}

void app_main(void)
{

    configuracion(); // Llama a la función de configuración del ADC


    while (true) // Bucle infinito
    {
        // Lee el valor crudo del canal 0 del ADC1
        esp_err_t ret = adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_raw);
        if (ret != ESP_OK) {
           // printf("ADC read failed\n");
            ESP_LOGE(ADC, "Ya valio\n");
            continue; // Si la lectura falla, continúa con la siguiente iteración
        }
        float voltage = adc_raw * (3.3 / 4095); // Convierte el valor ADC a voltaje (asumiendo Vref = 3.3V y resolución de 12 bits)
        // Imprime el valor leído por el ADC
        printf("%d,%.2f\n", adc_raw, voltage);
        ESP_LOGI(ADC, "Leyendo\n");
        vTaskDelay(pdMS_TO_TICKS(100)); // Espera 100 ms antes de la siguiente lectura
    }
}