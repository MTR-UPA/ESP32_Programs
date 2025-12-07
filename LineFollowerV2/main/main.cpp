#include "cabeceras.h"
#include "BTL.h"

BTL bt;
QTR8A sensor;
uint16_t sensor_values[SENSOR_COUNT];
uint16_t position;

bool start_bt = false;
bool cal_bt = false;

static const char *TAG1 = "ESP";
float KP = 0.2;

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

esp_err_t createSensor(void)
{
    // Initialize the sensor with the specified pins
    sensor.setSensorPins({D1, D2, D3, D4, D5, D6, D7, D8}, SENSOR_COUNT, THRESHOLD);
    sensor.setTypeAnalog();         // Set the sensor type to Analog
    sensor.setTimeout(2500);        // Set the timeout for Analog sensors
    sensor.setSamplesPerSensor(15); // Set the number of samples per sensor
    sensor.setEmitterPin(IR);       // Set the emitter pin for the sensor
    sensor.calibrate();                // Calibrate the sensor with emitters on
    return ESP_OK; // Return success
}

esp_err_t calibrateSensor(void)
{
    printf("Calibración iniciada... \n");
    sensor.calibrate(QTRReadMode::On);
    gpio_set_level(VERDE, 1);
    for (uint16_t i = 0; i < 150; ++i)
    {
        sensor.calibrate(); // Calibrate the sensor
        printf("%d\n", i);
        vTaskDelay(pdMS_TO_TICKS(10)); // Retardo de 10ms entre lecturas
    }
    printf("Calibración finalizada\n");
    gpio_set_level(VERDE, 0);

    return ESP_OK; // Return success
}

void getMaxMinCal(){
    for (int i = 0; i < SENSOR_COUNT; i++)
        printf("%d\t", sensor.calibrationOn.minimum[i]);
    printf("\n");
    for (int i = 0; i < SENSOR_COUNT; i++)
        printf("%d\t", sensor.calibrationOn.maximum[i]);
    printf("\n");
}

esp_err_t moveMotors(int16_t leftSpeed, int16_t rightSpeed)
{
    // Left motor
    if (leftSpeed > 0) {
        gpio_set_level(AIN1, 1);
        gpio_set_level(AIN2, 0);
    } else if (leftSpeed < 0) {
        gpio_set_level(AIN1, 0);
        gpio_set_level(AIN2, 1);
        leftSpeed = -leftSpeed;
    } else {
        gpio_set_level(AIN1, 0);
        gpio_set_level(AIN2, 0);
    }
    //TODO: Enviar señal PWM al motor izquierdo

    // Right motor
    if (rightSpeed > 0) {
        gpio_set_level(BIN1, 1);
        gpio_set_level(BIN2, 0);
    } else if (rightSpeed < 0) {
        gpio_set_level(BIN1, 0);
        gpio_set_level(BIN2, 1);
        rightSpeed = -rightSpeed;
    } else {
        gpio_set_level(BIN1, 0);
        gpio_set_level(BIN2, 0);
    }
    //TODO: Enviar señal PWM al motor derecho
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, leftSpeed, 0); // Actualiza el duty cycle
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, rightSpeed, 0); // Actualiza el duty cycle

    return ESP_OK;
}


void setupPWM(void)
{
    // Configuración del canal PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT, // Resolución de 8 bits
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000, // Frecuencia de 5 kHz
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false // No desconfigurar el temporizador
    };
    ledc_timer_config(&ledc_timer);


    // Configuración del canal B
    ledc_channel_config_t ledc_channel_B = {
        .gpio_num = PWMB, // Primero el GPIO
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD, // Deshabilitar el modo de sueño
        .flags = {
            .output_invert = 0 // No invertir la salida
        }};
    ledc_channel_config(&ledc_channel_B);

    ledc_fade_func_install(0); // Instala la función de desvanecimiento
}

void pruebaMotores(void)
{
    printf("Probando motores...\n");
    moveMotors(150, 150); // Mover ambos motores hacia adelante a velocidad 150
    vTaskDelay(pdMS_TO_TICKS(2000)); // Esperar 2 segundos
    
    moveMotors(0, 0); // Detener ambos motores
    printf("Prueba de motores finalizada.\n");
}


static void tarea_bluetooth(void *pvParameters)
{
    // Intentar hasta que esté conectado
    while (!bt.connectedToBluetooth())
    {
        bt.connectToBluetooth();
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    // Ya conectado: eliminar la propia tarea para no seguir intentándolo
    vTaskDelete(NULL);
}

void procesar_datos_bluetooth(const char* data, int len)
{
    if (data == NULL || len <= 0) return;
  
    // Crear copia sin salto de línea
    char comando[len + 1];
    memcpy(comando, data, len);
    comando[len] = '\0';
    
    // Remover salto de línea si existe
    if (comando[len - 1] == '\n') {
        comando[len - 1] = '\0';
    }
    if (comando[len - 2] == '\r') {
        comando[len - 2] = '\0';
    }
    
    // Procesar comandos

    if (strcmp(comando, "START") == 0) {
        ESP_LOGI(TAG1, "Comando: INICIAR");
        start_bt = true;
    }
    else if (strcmp(comando, "CAL") == 0) {
        ESP_LOGI(TAG1, "Comando: CALIBRAR");
        cal_bt = true;
    } 
    else if (strcmp(comando, "STOP") == 0) {
        ESP_LOGI(TAG1, "Comando: DETENER");
    }
    else if (strncmp(comando, "KP", 2) == 0) {
        const char* p = comando + 2; // después de "KP"
        if (*p != '\0') {
            char *endptr = nullptr;
            float val = strtof(p, &endptr);
            if (endptr != p) {
                KP = val;
                ESP_LOGI(TAG1, "Comando: KP - Ajustando KP a %f", KP);
            } else {
                ESP_LOGW(TAG1, "Valor de KP inválido: %s", p);
            }
        } else {
            ESP_LOGW(TAG1, "Comando KP sin valor");
        }
    }
    else {
        ESP_LOGW(TAG1, "Comando desconocido: %s", comando);
    }
}


extern "C" void app_main(void)
{
    printf("Iniciando programa...\n");
    configureGpio();
    createSensor();
    setupPWM();

        srand((unsigned) xTaskGetTickCount());

     //Inicia la tarea de conexion bluetooth
    bt.set_data_callback(procesar_datos_bluetooth);
    xTaskCreatePinnedToCore(tarea_bluetooth, "tarea_bluetooth", 4096, NULL, 1, NULL, 1);
    

    printf("%d \n", cal_bt);
    while(gpio_get_level(CAL) == 1 && !cal_bt) //  Boton sin presionar
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    calibrateSensor();
    getMaxMinCal();
    cal_bt = false;
    gpio_set_level(AZUL, 1);
    
    while(gpio_get_level(CAL) == 1 && !start_bt) //  Boton sin presionar
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    

    while(1){
        position = sensor.readLineBlack(sensor_values);
        for(int i = 0; i < SENSOR_COUNT; i++)
            printf("%d\t", sensor_values[i]);
        printf("P: %d\n", position);
        vTaskDelay(pdMS_TO_TICKS(100));


    }

    
}