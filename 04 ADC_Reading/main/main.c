#include <stdio.h> // Incluye la biblioteca estándar de entrada/salida

#include "freertos/FreeRTOS.h" // Incluye la biblioteca de FreeRTOS
#include "driver/adc.h"        // Incluye el controlador ADC para ESP32

int adc_value = 0; // Variable para almacenar el valor leído del ADC

void app_main(void)
{
    // Configura el canal 0 del ADC1 con una atenuación de 11dB
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    // Configura el ancho de datos del ADC a 12 bits
    adc1_config_width(ADC_WIDTH_BIT_12);

    while(true) // Bucle infinito
    {
        // Lee el valor crudo del canal 0 del ADC1
        adc_value = adc1_get_raw(ADC1_CHANNEL_0); 
        // Imprime el valor leído por el ADC
        printf("ADC Value is %d\n", adc_value);

        // Espera 100 ms antes de la siguiente lectura
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}