// Include Arduino framework
#include <Arduino.h>

// Include Wire Library for I2C Communications
#include <Wire.h>

#include "log.h"

const int MB = 1048576;
const int HALF_MB = MB / 2;
const int KB = 1024;
const int HALF_KB = KB / 2;

const char* enumToString(esp_chip_model_t model) {
  switch(model)
  {
    case CHIP_ESP32:   return PSTR("ESP32");
    case CHIP_ESP32S2: return PSTR("ESP32-S2");
    case CHIP_ESP32S3:  return PSTR("ESP32-S3");
    case CHIP_ESP32C3: return PSTR("ESP32-C3");
    case CHIP_ESP32H2:  return PSTR("ESP32-H2");
    default:    return PSTR("Unknown");
  }
}

void reportChipFeatures(uint32_t features)
{
  Slog.printf(PSTR("Features:\n"));
  if (CHIP_FEATURE_EMB_FLASH & features)
    Slog.printf(PSTR("  Embedded Flash Memory\n"));
  if (CHIP_FEATURE_WIFI_BGN & features)
    Slog.printf(PSTR("  2.4GHz WiFi\n")); 
  if (CHIP_FEATURE_BLE & features)
    Slog.printf(PSTR("  Bluetooth LE\n"));
  if (CHIP_FEATURE_BT & features)
    Slog.printf(PSTR("  Bluetooth Classic\n"));
  if (CHIP_FEATURE_IEEE802154 & features)
    Slog.printf(PSTR("  IEEE 802.15.4 (Low power, low data-rate wireless communication)\n"));             
  if (CHIP_FEATURE_EMB_PSRAM & features)
    Slog.printf(PSTR("  Embedded PSRAM\n"));    
}

void reportPSRAMsize()
{
    // Check if PSRAM is available
  if (esp_spiram_is_initialized())
  {
    Slog.printf(PSTR("PSRAM Size: %dMB\n"), esp_spiram_get_size() / MB);
  }
  else
  {
    Slog.printf(PSTR("No PSRAM available\n"));
  }

}

void ReportChipInfo()
{
  esp_chip_info_t chip_info;

  delay(1000);
  esp_chip_info(&chip_info);

  Slog.printf(PSTR("\nESP32 Chip Information:\n"));
  Slog.printf(PSTR("-----------------------\n"));
  Slog.printf(PSTR("Chip model: %s\n"), enumToString(chip_info.model));
  Slog.printf(PSTR("Number of cores: %d\n"), chip_info.cores);
  Slog.printf(PSTR("Revision: %d\n"), chip_info.revision);
  Slog.printf(PSTR("Embedded flash: %dMB\n"), (spi_flash_get_chip_size() + HALF_MB) / MB);
  Slog.printf(PSTR("Total heap size: %dKB\n"), (ESP.getHeapSize() + HALF_KB) / KB);
  Slog.printf(PSTR("Free heap size: %dKB\n"), (ESP.getFreeHeap() + HALF_KB) / KB);
  Slog.printf(PSTR("Largest free block: %dKB\n"), (ESP.getMaxAllocHeap() + HALF_KB) / KB);
  if (psramFound())
  {
    Slog.printf(PSTR("Total PSRAM size: %dMB\n"), (ESP.getPsramSize() + MB) / MB);
    Slog.printf(PSTR("Free PSRAM size: %dKB\n"), (ESP.getFreePsram() + KB) / KB);
  }
  else
  {
    Slog.printf(PSTR("PSRAM is not available\n"));
  }
  reportChipFeatures(chip_info.features);
  #if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
  Slog.printf(PSTR("Cache Log Bug: %s\n\n"), soc_has_cache_lock_bug() ? "Yes" : "No");
  #endif
}

void FindI2CDevices() 
{
  byte error, address;
  int nDevices;
  Slog.println(PSTR("\nI2C Scanner"));
  Slog.println(PSTR("Scanning..."));
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Slog.print(PSTR("I2C device found at address 0x"));
      if (address<16) {
        Slog.print(PSTR("0"));
      }
      Slog.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Slog.print(PSTR("Unknow error at address 0x"));
      if (address<16) {
        Slog.print(PSTR("0"));
      }
      Slog.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Slog.println(PSTR("No I2C devices found\n"));
  }
  else {
    Slog.println(PSTR("done\n"));
  }     

   delay(5000);     
}

String SecondsToHHMMSS(unsigned long seconds)
{
  unsigned long days = seconds / 86400;
  unsigned long hours = (seconds % 86400) / 3600;
  unsigned long minutes = (seconds % 3600) / 60;
  unsigned long sec = seconds % 60;
  
  char buffer[12]; // DD:HH:MM:SS is 11 chars + null terminator
  snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu:%02lu", days, hours, minutes, sec);
  
  return String(buffer);
}