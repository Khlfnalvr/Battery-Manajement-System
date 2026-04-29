#include <Arduino.h>
#include <math.h>

// === KONFIGURASI HARDWARE ===
#define NTC_PIN      14
#define SERIES_R     11000.0
#define ADC_MAX      4095.0
#define VCC          3.3
#define NUM_SAMPLES  10

// === KONSTANTA STEINHART-HART ===
const double SH_A =  4.952779e-04f;
const double SH_B =  3.941682e-04f;
const double SH_C = -9.295886e-07f;

float readTemperatureSH() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(NTC_PIN);
    delay(5);
  }
  float adcVal = sum / (float)NUM_SAMPLES;
  float voltage = (adcVal / ADC_MAX) * VCC;

  if (voltage <= 0 || voltage >= VCC) {
    Serial.println("ERROR,0,0,0,-999");
    return -999;
  }

  float R = SERIES_R * voltage / (VCC - voltage);
  double lnR  = log(R);
  double lnR3 = lnR * lnR * lnR;
  double invT  = SH_A + SH_B * lnR + SH_C * lnR3;
  float  tempC = (float)(1.0 / invT - 273.15);

  // Format CSV: timestamp_ms,ADC,Voltage_V,R_ohm,Temp_C
  Serial.print(millis());
  Serial.print(",");
  Serial.print(adcVal, 0);
  Serial.print(",");
  Serial.print(voltage, 4);
  Serial.print(",");
  Serial.print(R, 1);
  Serial.print(",");
  Serial.println(tempC, 2);

  return tempC;
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  delay(1000);

  // Header CSV untuk Excel
  Serial.println("Timestamp_ms,ADC_Raw,Voltage_V,R_NTC_ohm,Temp_C");
}

void loop() {
  readTemperatureSH();
  delay(1000);
}
