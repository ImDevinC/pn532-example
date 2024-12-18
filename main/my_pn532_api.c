#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>

// SPI pins
#define PIN_SPI_MISO 19
#define PIN_SPI_MOSI 23
#define PIN_SPI_CLK 18
#define PIN_SPI_CS 5

// PN532 Command
#define PN532_CMD_GET_FIRMWARE_VERSION 0x02

static const char *TAG = "PN532";

// SPI device handle
spi_device_handle_t spi;

// Function to initialize SPI interface
void spi_init() {
  esp_err_t ret;

  // Initialize SPI bus
  spi_bus_config_t buscfg = {
      .miso_io_num = PIN_SPI_MISO,
      .mosi_io_num = PIN_SPI_MOSI,
      .sclk_io_num = PIN_SPI_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 64,
  };
  ret = spi_bus_initialize(VSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
  ESP_ERROR_CHECK(ret);

  // Add SPI device to the bus
  spi_device_interface_config_t devcfg = {
      .clock_speed_hz = 500000,   // SPI speed 1 MHz
      .mode = 0,                  // SPI mode 0 (CPOL = 0, CPHA = 0)
      .spics_io_num = PIN_SPI_CS, // Chip select pin
      .queue_size = 1,
  };
  ret = spi_bus_add_device(VSPI_HOST, &devcfg, &spi);
  ESP_ERROR_CHECK(ret);
}

// Function to send PN532 commands and get response
esp_err_t pn532_send_command(uint8_t *command, size_t command_len,
                             uint8_t *response, size_t response_len) {
  esp_err_t ret;

  // Create buffer for the transaction (command + response)
  uint8_t tx_buffer[command_len];
  uint8_t rx_buffer[response_len];

  // Copy the command into tx_buffer
  memcpy(tx_buffer, command, command_len);

  spi_transaction_t trans = {
      .length = command_len * 8, // Command length in bits
      .tx_buffer = tx_buffer,
      .rx_buffer = rx_buffer,
  };

  ret = spi_device_transmit(spi, &trans);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to transmit SPI command");
    return ret;
  }

  // Copy the response
  memcpy(response, rx_buffer, response_len);

  return ESP_OK;
}

// Function to get firmware version from PN532
void pn532_get_firmware_version() {
  // Construct the "GetFirmwareVersion" command (PN532 to read firmware version)
  uint8_t command[] = {
      0x00, 0x00, 0xFF, // Preamble + Start Code
      0x01, 0x00,       // Length (1 byte command)
      0xD4, 0x02        // Command to get firmware version
  };

  uint8_t response[9]; // Expected response length: 9 bytes

  // Send the command and get the response
  esp_err_t ret =
      pn532_send_command(command, sizeof(command), response, sizeof(response));
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to get firmware version");
    return;
  }

  // Parse and check the response
  ESP_LOGI(TAG, "Received Response:");
  for (int i = 0; i < sizeof(response); i++) {
    ESP_LOGI(TAG, "0x%02X", response[i]);
  }

  // Check if the response is valid (Expected response: [0x00, 0x00, 0xFF, 0xXX,
  // 0xXX, 0xXX, 0xXX, Checksum])
  if (response[0] == 0x00 && response[1] == 0x00 && response[2] == 0xFF) {
    ESP_LOGI(TAG, "Firmware Version: 0x%02X", response[3]);
    ESP_LOGI(TAG, "Additional Firmware Info: 0x%02X 0x%02X 0x%02X", response[4],
             response[5], response[6]);
  } else {
    ESP_LOGE(TAG, "Invalid response from PN532");
  }
}

void app_main() {
  // Initialize SPI
  spi_init();

  // Keep running indefinitely
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    pn532_get_firmware_version();
  }
}
