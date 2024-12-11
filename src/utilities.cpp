// Include Arduino framework
#include <Arduino.h>

// Include Wire Library for I2C Communications
#include <Wire.h>

const char* enumToString(esp_chip_model_t model) {
  switch(model)
  {
    case CHIP_ESP32:   return "ESP32";
    case CHIP_ESP32S2: return "ESP32-S2";
    case CHIP_ESP32S3:  return "ESP32-S3";
    case CHIP_ESP32C3: return "ESP32-C3";
    case CHIP_ESP32H2:  return "ESP32-H2";
    default:    return "Unknown";
  }
}

void reportChipFeatures(uint32_t features)
{
  Serial.printf("Features:\n");
  if (CHIP_FEATURE_EMB_FLASH & features)
    Serial.printf("  Embedded Flash Memory\n");
  if (CHIP_FEATURE_WIFI_BGN & features)
    Serial.printf("  2.4GHz WiFi\n"); 
  if (CHIP_FEATURE_BLE & features)
    Serial.printf("  Bluetooth LE\n");
  if (CHIP_FEATURE_BT & features)
    Serial.printf("  Bluetooth Classic\n");
  if (CHIP_FEATURE_IEEE802154 & features)
    Serial.printf("  IEEE 802.15.4 (Low power, low data-rate wireless communication)\n");             
  if (CHIP_FEATURE_EMB_PSRAM & features)
    Serial.printf("  Embedded PSRAM\n");    
}

void reportPSRAMsize()
{
    // Check if PSRAM is available
  if (esp_spiram_is_initialized())
  {
    Serial.printf("PSRAM Size: %dMB\n", esp_spiram_get_size() / (1024 * 1024));
  }
  else
  {
    Serial.printf("No PSRAM available\n");
  }

}

const int MB = 1048576;
const int HALF_MB = MB / 2;
const int KB = 1024;
const int HALF_KB = KB / 2;

void ReportChipInfo()
{
  esp_chip_info_t chip_info;

  delay(1000);
  esp_chip_info(&chip_info);

  Serial.printf("\nESP32 Chip Information:\n");
  Serial.printf("-----------------------\n");
  Serial.printf("Chip model: %s\n", enumToString(chip_info.model));
  Serial.printf("Number of cores: %d\n", chip_info.cores);
  Serial.printf("Revision: %d\n", chip_info.revision);
  Serial.printf("Embedded flash: %dMB\n", (spi_flash_get_chip_size() + HALF_MB) / MB);
  Serial.printf("Total heap size: %dKB\n", (ESP.getHeapSize() + HALF_KB) / KB);
  Serial.printf("Free heap size: %dKB\n", (ESP.getFreeHeap() + HALF_KB) / KB);
  Serial.printf("Largest free block: %dKB\n", (ESP.getMaxAllocHeap() + HALF_KB) / KB);
  if (psramFound())
  {
    Serial.printf("Total PSRAM size: %dMB\n", (ESP.getPsramSize() + MB) / MB);
    Serial.printf("Free PSRAM size: %dKB\n", (ESP.getFreePsram() + KB) / KB);
  }
  else
  {
    Serial.printf("PSRAM is not available\n");
  }
  reportChipFeatures(chip_info.features);
  #if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
  Serial.printf("Cache Log Bug: %s\n\n", soc_has_cache_lock_bug() ? "Yes" : "No");
  #endif
}

void FindI2CDevices() 
{
  byte error, address;
  int nDevices;
  Serial.println("\nI2C Scanner");
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }     

   delay(5000);     
}