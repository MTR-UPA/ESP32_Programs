/* Programa principal para manejar la pantalla OLED */
/* La libreria ssd1306.h fue modificada a  partir del trabajo realizado por: 
https://github.com/voidlooprobotech/ESP32_ESP-IDF_Code
Se ajusta la libreria para solo usar I2C y se maneja la pantalla como un componente
de ESP-IDF.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ssd1306.h"
#include "font8x8_basic.h"
#include "driver/adc.h"

#define tag "OLED"
#define NUM_BARS 8
#define BAR_WIDTH 14      // ancho de cada barra en pixeles
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 48

SSD1306_t dev; //Asi se llama la pantalla

// Canales ADC1
adc1_channel_t adc_channels[NUM_BARS] = {
    ADC1_CHANNEL_0, // GPIO36
    ADC1_CHANNEL_1, // GPIO37
    ADC1_CHANNEL_2, // GPIO38
    ADC1_CHANNEL_3, // GPIO39
    ADC1_CHANNEL_4, // GPIO32
    ADC1_CHANNEL_5, // GPIO33
    ADC1_CHANNEL_6, // GPIO34
    ADC1_CHANNEL_7  // GPIO35
};


void draw_bars(uint16_t values[NUM_BARS]) {
    uint8_t buffer[SCREEN_WIDTH];
    for (int page = 0; page < dev._pages; page++) {
        memset(dev._page[page]._segs, 0x00, 128);
    }

    for (int i = 0; i < NUM_BARS; i++) {
        uint16_t height = values[i] * SCREEN_HEIGHT / 4095; // Escala de 0-4095 a 0-64 px

        for (int x = 0; x < BAR_WIDTH-2; x++) {
			for (int y = height; y > 0; y--) {
				_ssd1306_pixel(&dev, i * BAR_WIDTH + x, SCREEN_HEIGHT - y+16, false);
			}
        }
    }

    ssd1306_show_buffer(&dev);
}

void app_main(void)
{
	
	int center, top, bottom;
	char lineChar[20];

	   // Configurar ADC1 para cada canal
    for (int i = 0; i < NUM_BARS; i++) {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(adc_channels[i], ADC_ATTEN_DB_11);
    }
	 uint16_t adc_values[NUM_BARS];

	// Initialize the OLED display
	i2c_master_init(&dev, GPIO_NUM_21, GPIO_NUM_22, GPIO_NUM_15);


	ESP_LOGI(tag, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
	
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0x0f);
	while (1) {
        //Leer todos los canales
        for (int i = 0; i < NUM_BARS; i++) {
            adc_values[i] = adc1_get_raw(adc_channels[i]);
        }

		
        // Dibujar barras
        draw_bars(adc_values);

        vTaskDelay(pdMS_TO_TICKS(100)); // refresco cada 100 ms
    }


	/*
	ssd1306_display_text_box1(&dev, 3, 0, "Hello World!! This is a long", 16, 20, false, pdMS_TO_TICKS(100));
	vTaskDelay(pdMS_TO_TICKS(2000));
	ssd1306_display_text_x3(&dev, 3, "Hi!!", 5, false);
	vTaskDelay(pdMS_TO_TICKS(5000));
	ssd1306_fadeout(&dev);

	*/
	/*
	ssd1306_display_text(&dev, 0, "SSD1306 128x64", 14, false);
	ssd1306_display_text(&dev, 1, "ABCDEFGHIJKLMNOP", 16, false);
	ssd1306_display_text(&dev, 2, "abcdefghijklmnop",16, false);
	ssd1306_display_text(&dev, 3, "Hello World!!", 13, false);
	//ssd1306_clear_line(&dev, 4, true);
	//ssd1306_clear_line(&dev, 5, true);
	//ssd1306_clear_line(&dev, 6, true);
	//ssd1306_clear_line(&dev, 7, true);
	ssd1306_display_text(&dev, 4, "SSD1306 128x64", 14, true);
	ssd1306_display_text(&dev, 5, "ABCDEFGHIJKLMNOP", 16, true);
	ssd1306_display_text(&dev, 6, "abcdefghijklmnop",16, true);
	ssd1306_display_text(&dev, 7, "Hello World!!", 13, true);


	vTaskDelay(100 / portTICK_PERIOD_MS);
	
	// Display Count Down
	uint8_t image[24];
	memset(image, 0, sizeof(image));
	ssd1306_display_image(&dev, top, (6*8-1), image, sizeof(image));
	ssd1306_display_image(&dev, top+1, (6*8-1), image, sizeof(image));
	ssd1306_display_image(&dev, top+2, (6*8-1), image, sizeof(image));
	for(int font=0x39;font>0x30;font--) {
		memset(image, 0, sizeof(image));
		ssd1306_display_image(&dev, top+1, (7*8-1), image, 8);
		memcpy(image, font8x8_basic_tr[font], 8);
		if (dev._flip) ssd1306_flip(image, 8);
		ssd1306_display_image(&dev, top+1, (7*8-1), image, 8);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	
	// Scroll Up
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text(&dev, 0, "---Scroll  UP---", 16, true);
	//ssd1306_software_scroll(&dev, 7, 1);
	ssd1306_software_scroll(&dev, (dev._pages - 1), 1);
	for (int line=0;line<bottom+10;line++) {
		lineChar[0] = 0x01;
		sprintf(&lineChar[1], " Line %02d", line);
		ssd1306_scroll_text(&dev, lineChar, strlen(lineChar), false);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
	vTaskDelay(3000 / portTICK_PERIOD_MS);
	
	// Scroll Down
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text(&dev, 0, "--Scroll  DOWN--", 16, true);
	//ssd1306_software_scroll(&dev, 1, 7);
	ssd1306_software_scroll(&dev, 1, (dev._pages - 1) );
	for (int line=0;line<bottom+10;line++) {
		lineChar[0] = 0x02;
		sprintf(&lineChar[1], " Line %02d", line);
		ssd1306_scroll_text(&dev, lineChar, strlen(lineChar), false);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
	vTaskDelay(3000 / portTICK_PERIOD_MS);

	// Page Down
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text(&dev, 0, "---Page	DOWN---", 16, true);
	ssd1306_software_scroll(&dev, 1, (dev._pages-1) );
	for (int line=0;line<bottom+10;line++) {
		//if ( (line % 7) == 0) ssd1306_scroll_clear(&dev);
		if ( (line % (dev._pages-1)) == 0) ssd1306_scroll_clear(&dev);
		lineChar[0] = 0x02;
		sprintf(&lineChar[1], " Line %02d", line);
		ssd1306_scroll_text(&dev, lineChar, strlen(lineChar), false);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
	vTaskDelay(3000 / portTICK_PERIOD_MS);

	// Horizontal Scroll
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text(&dev, center, "Horizontal", 10, false);
	ssd1306_hardware_scroll(&dev, SCROLL_RIGHT);
	vTaskDelay(5000 / portTICK_PERIOD_MS);
	ssd1306_hardware_scroll(&dev, SCROLL_LEFT);
	vTaskDelay(5000 / portTICK_PERIOD_MS);
	ssd1306_hardware_scroll(&dev, SCROLL_STOP);
	
	// Vertical Scroll
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text(&dev, center, "Vertical", 8, false);
	ssd1306_hardware_scroll(&dev, SCROLL_DOWN);
	vTaskDelay(5000 / portTICK_PERIOD_MS);
	ssd1306_hardware_scroll(&dev, SCROLL_UP);
	vTaskDelay(5000 / portTICK_PERIOD_MS);
	ssd1306_hardware_scroll(&dev, SCROLL_STOP);
	
	// Invert
	ssd1306_clear_screen(&dev, true);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text(&dev, center, "  Good Bye!!", 12, true);
	vTaskDelay(5000 / portTICK_PERIOD_MS);
	*/

	// Fade Out
	ssd1306_fadeout(&dev);
	
	// Restart module
	esp_restart();
}
