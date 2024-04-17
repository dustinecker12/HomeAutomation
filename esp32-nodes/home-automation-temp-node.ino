#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <math.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Temp sensor variables
#define ADC_VREF_mV 3300.0
#define ADC_RESOLUTION 4096.0
#define PIN_LM35 A10

// Network credentials
#define WIFI_SSID "XXX"
#define WIFI_PASSWORD "XXX"

// Firebase API key
#define API_KEY "XXX"

// RTDB URL
#define DATABASE_URL "XXX"

// Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

int temp = 0;
bool signupOK = false;

void setup() {
  // Connect to network
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Assign API key and RTDB URL
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Sign up as anonymous user
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  temp = getTemp();

  if (Firebase.ready() && signupOK) {
    // Write an Int number on the database path acParams/actualTemp
    if (Firebase.RTDB.setInt(&fbdo, "acParams/actualTemp", temp)) {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.println("°F");
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}

/* 
 * Reads the temperature every 1 second and averages it out over a minute.
 * The temperature sensor is very cheap and sporatic, so this function is to
 * try and smooth out the average temperature.
 */
int getTemp() {
  // Initialize array of 60 temp readings
  float temps[60];

  // Collect a temp reading, convert to °F, then wait 1 second.
  for (int i = 0; i < 60; i++) {
    int adcVal = analogRead(PIN_LM35);
    float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
    float tempC = milliVolt / 10;
    float tempF = tempC * 9 / 5 + 32;
    temps[i] = tempF;
    delay(100);
  }

  // Average temperature and return
  float avgTemp;
  for (int i = 0; i < 60; i++) {
    avgTemp += temps[i];
  }
  avgTemp /= 60;

  return floor(avgTemp);
}
