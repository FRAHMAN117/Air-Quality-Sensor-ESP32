#include "CSL_AQS_ESP32_V1.h"

/* INITIALIZE SD CARD */
void initializeSD() {

  if (!SD.begin()) {  // see if the card is present and can be initialized:
    Serial.println("Card mount failed");
    return;
  } else {
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
      Serial.println("No SD card attached");
      return;
    }

    Serial.print("Starting SD card... ");

    char filename[] = "/LOG0000.TXT";  // create a new file//increase 0000
    for (uint16_t i = 0; i <= 9999; i++) {
      filename[4] = i / 1000 + '0';  //48 ascii character value of 0 //integer division by
      filename[5] = i / 100 - 10 * (i / 1000) + '0';
      filename[6] = i / 10 - 10 * (i / 100) + '0';  //integer division 100
      filename[7] = i % 10 + '0';                   //modulo 10
      if (!SD.exists(filename)) {                   // only open a new file if it doesn't exist
        logfile = SD.open(filename, FILE_WRITE);
        break;  // leave the loop
      }
    }
    if (!logfile) {
      Serial.println("Couldn't create file");
    } else {
      Serial.print("Logging to file: ");
      Serial.println(filename);
      delay(200);
      // display.print("logging to file: ");
      // display.println(filename);
      // display.display();
    }
  }
  delay(1000);
}

/* INITIALIZE RTC */
void initializeRTC() {

  Serial.print("Starting RTC ... ");
  Wire.begin();  // connect to RTC
  if (!rtc.begin()) {
    Serial.println("RTC failed");
    display.println("RTC failed");
    display.display();
  } else {
    Serial.println("RTC connected");
    display.println("RTC connected");
    display.display();
  }
  // TO SET TIME at compile : run once to syncro then run again with line commented out
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}
