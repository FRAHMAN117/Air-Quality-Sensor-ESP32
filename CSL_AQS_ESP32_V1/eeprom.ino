
#include "EEPROM.h"
#include "CSL_AQS_ESP32_V1.h"

void initializeEEPROM() {

  Serial.print("Starting EEPROM ... ");
  if (!EEPROM.begin(512)) {
    Serial.println("Failed to initialize EEPROM. Restarting...");
    delay(1000);
    ESP.restart();
  }

  uint8_t val = EEPROM.readByte(0);
  if (!val) {  // never been initialized
    memset(&provisionInfo, 0, sizeof(provisionInfo));
    //EEPROM.writeByte(0, 0xAA);
    EEPROM.commit();
    Serial.printf("EEPROM zeroed. Check value: 0x%X\n", val);
    return;
  } else  {
    Serial.printf("Reading EEPROM provisioning info that may work. Check value: 0x%X\n", val);
    EEPROM.readBytes(0, &provisionInfo, sizeof(provisionInfo));
    return;
  }
}

void readProvisionInfo() {
  EEPROM.readBytes(0, &provisionInfo, sizeof(provisionInfo));
  return;
}

void storeProvisionInfo() {
  EEPROM.writeBytes(0, &provisionInfo, sizeof(provisionInfo));
  EEPROM.commit();
  // EEPROM.readBytes(0, &provisionInfo, sizeof(provisionInfo));
  // Serial.printf("storeProvisionInfo: valid %u, ssid %s, psscd %s, gsid %s, nowifi %u\n",
  //                 provisionInfo.valid,provisionInfo.ssid,provisionInfo.passcode,
  //                 provisionInfo.gsid,provisionInfo.noWifi);
  return;
}
