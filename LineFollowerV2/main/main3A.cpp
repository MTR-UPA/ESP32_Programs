#include "cabeceras.h"

static const char *TAG = "ESP";
static const char *NVS = "MEM";
static const char *BOTON = "CAL";
uint16_t sensor_values[SENSOR_COUNT];
QTR8A sensor;
bool calibrationLoaded = false;

esp_err_t configureGpio(void)
{
    // Configure GPIO pins for input and output modes
    gpio_reset_pin(VERDE); // Reset LED pin
    gpio_reset_pin(ROJO);  // Reset LED1 pin
    gpio_reset_pin(RDY);   // Reset button pin
    gpio_reset_pin(AZUL);  // Reset button pin
    gpio_reset_pin(AIN1);  // Reset AIN1 pin
    gpio_reset_pin(AIN2);  // Reset AIN2 pin
    gpio_reset_pin(PWMA);  // Reset PWMA pin
    gpio_reset_pin(BIN1);  // Reset BIN1 pin
    gpio_reset_pin(BIN2);  // Reset BIN2 pin
    gpio_reset_pin(PWMB);  // Reset PWMB pin
    gpio_reset_pin(IR);    // Reset IR pin
    gpio_reset_pin(ISVM);  // Reset ISVM pin
    gpio_set_direction(VERDE, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROJO, GPIO_MODE_OUTPUT);
    gpio_set_direction(AZUL, GPIO_MODE_OUTPUT);
    gpio_set_direction(RDY, GPIO_MODE_INPUT);  // Set button pin as input
    gpio_set_pull_mode(RDY, GPIO_PULLUP_ONLY); // Activa el pull-up interno
    gpio_set_direction(CAL, GPIO_MODE_INPUT);  // Set button pin as input
    gpio_set_pull_mode(CAL, GPIO_PULLUP_ONLY); // Activa el pull-up interno
    gpio_set_direction(ISVM, GPIO_MODE_INPUT); // Set button pin as input
    gpio_set_direction(IR, GPIO_MODE_OUTPUT);  // Set button pin as input
    gpio_set_direction(AIN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(AIN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(PWMA, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(PWMB, GPIO_MODE_OUTPUT);
    return ESP_OK; // Return success
}

extern "C" void app_main(void)
{
   
}