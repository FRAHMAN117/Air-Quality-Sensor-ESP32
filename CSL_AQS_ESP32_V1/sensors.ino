#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SensirionI2CSen5x.h> 
#include "SparkFun_SCD4x_Arduino_Library.h"

#include "CSL_AQS_ESP32_V1.h"

// Sensors
SensirionI2CSen5x SEN55;
Adafruit_BME280 bme;
//SCD30 CO2sensor;
SCD4x CO2sensor(SCD4x_SENSOR_SCD41);

/* BME280 TEMP, PRESS, RH BOARD FROM ADAFRUIT USING 'Adafruit_BME280.h' */

void initializeBME() {

  Serial.print("Starting BME280 Temp Press RH sensor ... ");
  // You can also pass in a Wire library object like &Wire2
  // bmeStatus = bme.begin(0x76, &Wire2)
  if (!bme.begin()) {
    Serial.println("BME280 not detected");
    display.println("BME280 not detected");
    display.display();
  } else {
    Serial.println("BME280 connected");
    display.println("BME280 connected");
    display.display();
  }
}

String readBME() {
  sensorData.Tbme = bme.readTemperature();
  sensorData.Pbme = bme.readPressure() / 100;  // for hPa
  sensorData.RHbme = bme.readHumidity();

  String bmeString = String(sensorData.Tbme) + String(", ") + String(sensorData.Pbme) + String(", ") + String(sensorData.RHbme) + String(", ");
  return bmeString;
}

/* SCD41 CO2 SENSOR FROM SENSIRION USING "SparkFun_SCD4x_Arduino_Library.h"*/

void initializeSCD41() {
  Serial.print("Starting SCD41 CO2 sensor ... ");
  //mySensor.enableDebugging(); // Uncomment this line to get helpful debug messages on Serial
  if (CO2sensor.begin(false, false, false) == false)  // measBegin, autoCal, skipStopPeriodicMeasurements i.e. Do not start periodic measurements
  {
    Serial.println("SCD41 sensor not detected");
    display.println("SCD41 sensor not detected");
    display.display();
  }
  if (!CO2sensor.measureSingleShot()) {
    Serial.println("SCD41 measureSingleShot failed");
    display.println("SCD41 measureSingleShot failed");
    display.display();
  } else
    Serial.println("SCD41 connected");
  display.println("SCD41 connected");
  display.display();
}

String readSCD41() {
  CO2sensor.measureSingleShot();        // Request fresh data (should take 5 seconds)
  while (!CO2sensor.readMeasurement())  // readMeasurement will return true when fresh data is available
  {
    Serial.print("+");
    delay(500);
  }
  Serial.println();
  sensorData.CO2 = CO2sensor.getCO2();
  sensorData.Tco2 = CO2sensor.getTemperature();
  sensorData.RHco2 = CO2sensor.getHumidity();

  return (String(sensorData.CO2) + String(", ") + String(sensorData.Tco2) + String(", ") + String(sensorData.RHco2) + String(", "));
}

/* SEN55 PM VOC NOX SENSOR FROM SENSIRION USING "SensirionI2CSen5x.h" */

#define MAXBUF_REQUIREMENT 48
#if ((defined(I2C_BUFFER_LENGTH) && (I2C_BUFFER_LENGTH >= MAXBUF_REQUIREMENT)) || (defined(BUFFER_LENGTH) && BUFFER_LENGTH >= MAXBUF_REQUIREMENT))
#define USE_PRODUCT_INFO
#endif

void initializeSEN55() {
  Serial.print("Starting SEN55 PM VOC NOX sensor ... ");

  Wire.begin();
  SEN55.begin(Wire);

  uint16_t error;
  char errorMessage[256];
  error = SEN55.deviceReset();
  if (error) {
    Serial.print("Error trying to execute deviceReset(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }

  float tempOffset = 0.0;
  error = SEN55.setTemperatureOffsetSimple(tempOffset);
  if (error) {
    Serial.print("Error trying to execute setTemperatureOffsetSimple(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } else {
    Serial.printf("Temp Offset set to %f C... ", tempOffset);
  }
  error = SEN55.startMeasurement();
  if (error) {
    Serial.print("Error trying to execute startMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } else {
    Serial.println("SEN55 connected");
    display.println("SEN55 connected");
    display.display();
  }
}

String readSEN55() {
  uint16_t error;
  char errorMessage[256];
  delay(1000);

  error = SEN55.readMeasuredValues(
          sensorData.mPm1_0,sensorData.mPm2_5,sensorData.mPm4_0,
          sensorData.mPm10,sensorData.RHsen,sensorData.Tsen,
          sensorData.VOCs,sensorData.NOx);

  if (error) {
    Serial.print("Error trying to execute readMeasuredValues(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }

  String SEN55_String = String(sensorData.mPm1_0) + String(", ") + String(sensorData.mPm2_5) + String(", ") + String(sensorData.mPm4_0) + String(", ")
                        + String(sensorData.mPm10) + String(", ") + String(sensorData.RHsen) + String(", ") + String(sensorData.Tsen) + String(", ") 
                        + String(sensorData.VOCs) + String(", ") + String(sensorData.NOx) + String(", ");

  return SEN55_String;
}
