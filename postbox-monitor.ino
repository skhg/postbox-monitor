/**
 * Copyright 2022 Jack Higgins : https://github.com/skhg
 * All components of this project are licensed under the MIT License.
 * See the LICENSE file for details.
 */

/**
 * Network Configuration
 */
#define WIFI_CONNECT_TIMEOUT_SECONDS 10
#define SERVER_ROOT "http://192.168.178.29:8080/postbox"
#define WIFI_CLIENT_HOSTNAME "postbox"

/**
 * Ping interval in seconds
 */
#define PING_INTERVAL_SECONDS 86400  // 1 day = 86400 = 60 * 60 * 24









/**
 * Http defaults which never change
 */

#define HTTP_OK 200
#define HTTP_CONTENT_TYPE_HEADER "Content-Type"
#define HTTP_CONTENT_LENGTH_HEADER "Content-Length"
#define HTTP_APPLICATION_JSON_CONTENT_TYPE "application/json"

/**
 * Data points required for voltage measurement
 */
#define STABILISING_DATA_POINTS 100
#define STABILISING_POINT_INTERVAL 10

/**
 * Pin Mappings and hardware configuration
 */

#define FLAP_BUTTON_MASK 0x1000000000  // Button connected to GPIO36 (A0)
#define DOOR_BUTTON_MASK 0x8000000000  // Button connected to GPIO39 (A1)
#define uS_TO_S_FACTOR 1000000ULL  // Conversion from microseconds to seconds
#define FLAP A0
#define DOOR A1
#define VOLTAGE_PIN A2








/**
 * States and event types
 */
enum DOOR_STATES {
  OPEN,
  CLOSED
};

enum NOTIFY_TYPES {
  NO_EVENT,
  BOOT,
  DELIVERED,
  RETRIEVED,
  PING
};







/**
 * Variables which are persistent during deep sleep
 */
RTC_DATA_ATTR long _retryCount = 0;
RTC_DATA_ATTR int _triggerSwitch = DOOR;
RTC_DATA_ATTR int _waitingForState = CLOSED;
RTC_DATA_ATTR NOTIFY_TYPES _pendingTransmission = NO_EVENT;








#include <home_wifi.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

WiFiClient WIFI_CLIENT;
HTTPClient HTTP_CLIENT;

void setup() {
  Serial.begin(115200);

  switch (esp_sleep_get_wakeup_cause()) {
    case ESP_SLEEP_WAKEUP_EXT1: {
      Serial.println("Woke due to sensor event");
      onSensorWake();
      break;
    }
    case ESP_SLEEP_WAKEUP_TIMER: {
      Serial.println("Woke due to timer event");
      onTimerWake();
      break;
    }
    default: {
      Serial.println("Woke due to boot event");
      onBootWake();
      break;
    }
  }

  scheduleNextPing();

  Serial.flush();
  WiFi.disconnect();

  esp_deep_sleep_start();
}

void loop() { }

void onTimerWake() {
  executeSchedule();
  waitFor(_triggerSwitch, _waitingForState);
}

void onBootWake() {
  _retryCount = 0;
  notify(BOOT);
  waitFor(FLAP, OPEN);
}

void onSensorWake() {
  _retryCount = 0;
  switch (_triggerSwitch) {
    case FLAP: flapOpened(); break;
    case DOOR: doorOpened(); break;
  }
}

void executeSchedule() {
  if (_pendingTransmission != NO_EVENT) {
    _retryCount++;
    notify(_pendingTransmission);
  }
}

void scheduleNextPing() {
  if (_pendingTransmission != NO_EVENT) {
    Serial.print("Not scheduling a ping because a ");
    Serial.print(notifyTypeToString(_pendingTransmission));
    Serial.println(" transmission is already pending.");
    scheduleRetry(_pendingTransmission);
    return;
  }

  _pendingTransmission = PING;
  _retryCount = -1;

  Serial.print("Scheduling next ping for ");
  Serial.print(PING_INTERVAL_SECONDS);
  Serial.println(" seconds from now...");

  uint64_t microsecondsToGo = PING_INTERVAL_SECONDS * uS_TO_S_FACTOR;
  esp_sleep_enable_timer_wakeup(microsecondsToGo);
}

void scheduleRetry(NOTIFY_TYPES notifyType) {
  Serial.println("Retry scheduled");
  _pendingTransmission = notifyType;

  long t1 = 0;
  long t2 = 1;
  long nth = 1;

  for (long i = 2; i <= _retryCount; i++) {
    nth = t1 + t2;
    t1 = t2;
    t2 = nth;

    if (nth >= PING_INTERVAL_SECONDS) {
      break;
    }
  }

  Serial.print("Scheduling retry for ");
  Serial.print(nth);
  Serial.println(" seconds from now...");

  uint64_t delayTimeMicroseconds = nth * uS_TO_S_FACTOR;

  esp_sleep_enable_timer_wakeup(delayTimeMicroseconds);
}

void clearRetry() {
  _pendingTransmission = NO_EVENT;
  _retryCount = 0;
}

String buildMessage() {
  DynamicJsonDocument json(JSON_OBJECT_SIZE(2));

  json["deviceVoltageMeasurement"] = voltageReading();
  json["retryCount"] = _retryCount;

  String serialised;
  serializeJson(json, serialised);

  return serialised;
}

void notify(NOTIFY_TYPES notifyType) {
  if (!connectToWifi()) {
    scheduleRetry(notifyType);
    return;
  }

  String path;

  switch (notifyType) {
    case NO_EVENT: return;
    case BOOT: path = "/boot"; break;
    case DELIVERED: path = "/delivered"; break;
    case RETRIEVED: path = "/retrieved"; break;
    case PING: path = "/ping"; break;
    default: return;
  }

  String jsonString = buildMessage();

  Serial.print("Posting ");
  Serial.print(jsonString);
  Serial.print(" to ");
  Serial.println(path);

  HTTP_CLIENT.begin(WIFI_CLIENT, String(SERVER_ROOT) + path);
  HTTP_CLIENT.addHeader(HTTP_CONTENT_TYPE_HEADER, HTTP_APPLICATION_JSON_CONTENT_TYPE);
  HTTP_CLIENT.addHeader(HTTP_CONTENT_LENGTH_HEADER, String(jsonString.length()));
  int result = HTTP_CLIENT.POST(jsonString);

  if (result == HTTP_OK) {
    Serial.println("HTTP POST completed successfully!");
    clearRetry();
  } else {
    Serial.print("HTTP POST failed with code "); Serial.println(result);
    scheduleRetry(notifyType);
  }
}

void flapOpened() {
  Serial.println("Post arrived!");
  notify(DELIVERED);
  waitFor(DOOR, OPEN);
}

void doorOpened() {
  switch (_waitingForState) {
    case OPEN: {
      Serial.println("Door opened!");
      notify(RETRIEVED);
      waitFor(DOOR, CLOSED);
      break;
    } case CLOSED: {
      Serial.println("Door closed!");
      waitFor(FLAP, OPEN);
      break;
    }
  }
}

void waitFor(int switchName, int nextState) {
  Serial.print("Waiting for ");

  _triggerSwitch = switchName;
  _waitingForState = nextState;

  esp_sleep_ext1_wakeup_mode_t wakeMode;


  switch (nextState) {
    case OPEN: {
      wakeMode = ESP_EXT1_WAKEUP_ALL_LOW;
      Serial.print("opening");
      break;
    } case CLOSED: {
      wakeMode = ESP_EXT1_WAKEUP_ANY_HIGH;
      Serial.print("closing");
      break;
    }
  }

  Serial.print(" of ");

  switch (switchName) {
    case DOOR: {
      esp_sleep_enable_ext1_wakeup(DOOR_BUTTON_MASK, wakeMode);
      Serial.print("DOOR");
      break;
    }
    case FLAP: {
      esp_sleep_enable_ext1_wakeup(FLAP_BUTTON_MASK, wakeMode);
      Serial.print("FLAP");
      break;
    }
  }

  Serial.println(".");
}

int voltageReading() {
  pinMode(VOLTAGE_PIN, INPUT);

  int voltageValue = 0;
  for (int i=0; i < STABILISING_DATA_POINTS; i++) {
    voltageValue = voltageValue + analogRead(VOLTAGE_PIN);
    delay(STABILISING_POINT_INTERVAL);
  }

  voltageValue = voltageValue / STABILISING_DATA_POINTS;
  return voltageValue;
}

/**
 * Helper methods
 */

String notifyTypeToString(NOTIFY_TYPES notifyType) {
  switch (notifyType) {
    case NO_EVENT: return "no event";
    case BOOT: return "boot";
    case DELIVERED: return "delivered";
    case RETRIEVED: return "retrieved";
    case PING: return "ping";
    default: return "Unknown";
  }
}

/**
 * WiFi management
 */

boolean connectToWifi() {
  String wifiConnectionInfo = "Connecting to WiFi";


  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.setHostname(WIFI_CLIENT_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int connectAttempts = 0;
  int connectRetryInterval = 500;
  int failureCountdown = WIFI_CONNECT_TIMEOUT_SECONDS * 1000;

  while (WiFi.status() != WL_CONNECTED) {
    delay(connectRetryInterval);

    Serial.print(".");

    failureCountdown = failureCountdown - connectRetryInterval;

    if (failureCountdown < 0) {
      Serial.println("");
      Serial.println("Failed to connect.");
      return false;
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  return true;
}
