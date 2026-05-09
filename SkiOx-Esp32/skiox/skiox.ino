#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== WIFI =====
const char* ssid = "Esp";
const char* password = "12345678";
const char* serverURL = "http://10.157.36.189:8000/api/data/";

// ===== OLED =====
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ===== SENSOR =====
MAX30105 sensor;

// ===== PINS =====
#define LED_PIN 19
#define BUZZER_PIN 18

// ===== PWM =====
#define BUZZ_FREQ 1000
#define BUZZ_RES 8

// ===== BUFFERS =====
#define BUFFER_SIZE 50
uint32_t irBuffer[BUFFER_SIZE];
uint32_t redBuffer[BUFFER_SIZE];

int32_t spo2;
int32_t heartRate;
int8_t validSPO2;
int8_t validHeartRate;

unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(LED_PIN, OUTPUT);

  // ✅ ESP32 PWM (NEW API)
  ledcAttach(BUZZER_PIN, BUZZ_FREQ, BUZZ_RES);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);

  display.setCursor(10, 20);
  display.println("Connecting WiFi");
  display.display();

  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  display.clearDisplay();
  display.setCursor(10, 20);
  display.println("WiFi OK");
  display.display();
  delay(1000);

  // Sensor
  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    display.clearDisplay();
    display.setCursor(10, 20);
    display.println("Sensor Error");
    display.display();
    while (1);
  }

  sensor.setup();
  sensor.setPulseAmplitudeRed(0x0A);
  sensor.setPulseAmplitudeGreen(0);
}

void loop() {

  long ir = sensor.getIR();

  // ===== NO FINGER =====
  if (ir < 30000) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 25);
    display.println("NO FINGER");
    display.display();

    digitalWrite(LED_PIN, LOW);
    ledcWrite(BUZZER_PIN, 0);

    delay(300);
    return;
  }

  // ===== READ SENSOR =====
  for (byte i = 0; i < BUFFER_SIZE; i++) {
    while (!sensor.available())
      sensor.check();

    redBuffer[i] = sensor.getRed();
    irBuffer[i] = sensor.getIR();
    sensor.nextSample();
  }

  // ===== CALCULATE =====
  maxim_heart_rate_and_oxygen_saturation(
    irBuffer, BUFFER_SIZE,
    redBuffer,
    &spo2, &validSPO2,
    &heartRate, &validHeartRate
  );

  // ===== FIX INVALID VALUES =====
  if (!validSPO2 || spo2 == -999 || spo2 > 100 || spo2 < 0) {
    spo2 = -1;  // mark invalid
  }

  if (!validHeartRate || heartRate == -999 || heartRate > 220 || heartRate < 30) {
    heartRate = -1;
  }

  // ===== DISPLAY =====
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SkiOx Monitor");

  display.setTextSize(2);

  display.setCursor(0, 16);
  display.print("Sp:");
  if (spo2 == -1) {
    display.print("--");
  } else {
    display.print(spo2);
  }

  display.setCursor(0, 40);
  display.print("HR:");
  if (heartRate == -1) {
    display.print("--");
  } else {
    display.print(heartRate);
  }

  display.display();

  // ===== ALERT =====
  if (spo2 != -1 && spo2 < 90) {
    digitalWrite(LED_PIN, HIGH);
    ledcWrite(BUZZER_PIN, 200);
  } else {
    digitalWrite(LED_PIN, LOW);
    ledcWrite(BUZZER_PIN, 0);
  }

  // ===== SEND TO DJANGO =====
  if (millis() - lastSend > 5000) {
    lastSend = millis();

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      String json = "{\"spo2\": " + String(spo2) +
                    ", \"hr\": " + String(heartRate) + "}";

      http.begin(serverURL);
      http.addHeader("Content-Type", "application/json");

      int response = http.POST(json);

      Serial.print("Server: ");
      Serial.println(response);

      http.end();
    }
  }

  // DEBUG
  Serial.print("IR: ");
  Serial.print(ir);
  Serial.print(" | SpO2: ");
  Serial.print(spo2);
  Serial.print(" | HR: ");
  Serial.println(heartRate);

  delay(500);
}