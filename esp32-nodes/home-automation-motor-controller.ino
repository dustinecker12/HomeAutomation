#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <math.h>
#include <Stepper.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

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

int actualTemp = 0;
int targetTemp = 0;
bool onOffButton = false;
bool overrideEnabled = false;
bool acOn = false;
bool signupOK = false;
const int stepsPerRevolution = 512;

Stepper myStepper = Stepper(stepsPerRevolution, 7, 9, 8, 10);

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
  readDB();
  checkCondition();

  delay(5000);
}

// Read all variables from Firebase RTDB
void readDB() {
  if (Firebase.ready() && signupOK) {
    if (Firebase.RTDB.getInt(&fbdo, "acParams/actualTemp")) {
      actualTemp = fbdo.to<int>();
      // Serial.print("Actual Temperature: ");
      // Serial.print(actualTemp);
      // Serial.println("°F");
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.getInt(&fbdo, "acParams/targetTemp")) {
      targetTemp = fbdo.to<int>();
      // Serial.print("Target Temperature: ");
      // Serial.print(targetTemp);
      // Serial.println("°F");
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.getBool(&fbdo, "acParams/onOffButton")) {
      onOffButton = fbdo.to<bool>();
      // Serial.print("onOffButton: ");
      // Serial.println(onOffButton);
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.getBool(&fbdo, "acParams/overrideEnabled")) {
      overrideEnabled = fbdo.to<bool>();
      // Serial.print("overrideEnabled: ");
      // Serial.println(overrideEnabled);
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.getBool(&fbdo, "acState/acOn")) {
      acOn = fbdo.to<bool>();
      // Serial.print("acOn: ");
      // Serial.println(acOn);
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}

void checkCondition() {
  // If override is enabled, ignore all other conditions, else check remaining conditions
  if (overrideEnabled) {
    Serial.println("Override is enabled");
    if (onOffButton && !acOn) {
      turnAcOn();
    } else if (!onOffButton && acOn) {
      turnAcOff();
    }
  } else {
    Serial.println("Override is disabled");
    if (targetTemp < actualTemp && !acOn) {
      turnAcOn();
    } else if (targetTemp >= actualTemp && acOn) {
      turnAcOff();
    }
  }
}

// Turn motor 90 degrees clockwise (knob to right) and write to db
void turnAcOn() {
  Serial.println("Turning AC on");
  acOn = true;

  if (!Firebase.RTDB.setBool(&fbdo, "acState/acOn", acOn)) {
    Serial.println("Failed: " + fbdo.errorReason());
  } else {
    myStepper.setSpeed(50);
    myStepper.step(-stepsPerRevolution);
  }
}

// Turn motor 90 degrees counter-clockwise (knob to left) and write to db
void turnAcOff() {
  Serial.println("Turning AC off");

  acOn = false;

  if (!Firebase.RTDB.setBool(&fbdo, "acState/acOn", acOn)) {
    Serial.println("Failed: " + fbdo.errorReason());
  } else {
    myStepper.setSpeed(50);
    myStepper.step(stepsPerRevolution);
  }
}
