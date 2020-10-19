#define STABILISING_DATA_POINTS 100
#define STABILISING_POINT_INTERVAL 1

/**
 * Network Configuration
 */
#define WIFI_CONNECT_TIMEOUT_SECONDS 10
#define SERVER_ROOT "http://192.168.178.20:8080/postbox"
#define WIFI_CLIENT_HOSTNAME "postbox"

/**
 * Http defaults
 */

#define HTTP_OK 200
#define HTTP_CONTENT_TYPE_HEADER "Content-Type"
#define HTTP_CONTENT_LENGTH_HEADER "Content-Length"
#define HTTP_APPLICATION_JSON_CONTENT_TYPE "application/json"

 
/**
 * Pin Mappings and hardware configuration
 */

#define FLAP_BUTTON_MASK 0x1000000000 //Button connected to GPIO36 (A0)
#define DOOR_BUTTON_MASK 0x8000000000 //Button connected to GPIO39 (A1)

#define FLAP A0
#define DOOR A1
#define VOLTAGE_PIN A2

enum DOOR_STATES {
  OPEN,
  CLOSED
};

enum EVENT_TO_TRANSMIT {
  NO_EVENT,
  BOOT,
  DELIVERED,
  RETRIEVED,
  PING
};

RTC_DATA_ATTR int _retryCount = 0;
RTC_DATA_ATTR int _triggerSwitch = DOOR;
RTC_DATA_ATTR int _waitingForState = CLOSED;
RTC_DATA_ATTR EVENT_TO_TRANSMIT _waitingToTransmit = BOOT;

#include <home_wifi.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

WiFiClient WIFI_CLIENT;
HTTPClient HTTP_CLIENT;

void setup(){
  Serial.begin(115200);
  
  switch(_triggerSwitch){
    case FLAP: flapOpened(); break;
    case DOOR: doorOpened(); break;
  }

  //todo set up a retry if wifi connection fails. sleep for a few minutes and repeat until successful.
//// use a retryCounter as well and include in the http method

  Serial.flush();
  WiFi.disconnect();
  
  esp_deep_sleep_start();
}

void loop(){}

String buildMessage(){
  DynamicJsonDocument json(JSON_OBJECT_SIZE(2));
  
  json["deviceVoltageMeasurement"] = voltageReading();
  json["retryCount"] = _retryCount;

  String serialised;

  serializeJson(json, serialised);

  Serial.println(serialised);
  
  return serialised;
}

boolean postHttp(String path){
  String jsonString = buildMessage();
  
  HTTP_CLIENT.begin(WIFI_CLIENT, String(SERVER_ROOT) + path);
  HTTP_CLIENT.addHeader(HTTP_CONTENT_TYPE_HEADER, HTTP_APPLICATION_JSON_CONTENT_TYPE);
  HTTP_CLIENT.addHeader(HTTP_CONTENT_LENGTH_HEADER, String(jsonString.length()));
  int result = HTTP_CLIENT.POST(jsonString);

  Serial.print("HTTP POST Completed with code "); Serial.println(result);
  
  return result == HTTP_OK;
}

void flapOpened(){
  Serial.println("Post arrived!");

  if(connectToWifi()){
    postHttp("/delivered");
    //todo handle the fail case of no wifi or non ok response, set up a retry pause
  }
  waitFor(DOOR, OPEN);
}

void doorOpened(){
  switch(_waitingForState){
    case OPEN: {
      Serial.println("Door opened!");
      if(connectToWifi()){
        postHttp("/retrieved");
      }
      waitFor(DOOR, CLOSED);
      break;
    }
    case CLOSED: {
      Serial.println("Door closed!");
      waitFor(FLAP, OPEN);
      break;
    }
  }
}

void waitFor(int switchName, int nextState){
  Serial.print("Waiting for ");
  
  _triggerSwitch = switchName;
  _waitingForState = nextState;

  esp_sleep_ext1_wakeup_mode_t wakeMode;

  
  switch(nextState){
    case OPEN: wakeMode = ESP_EXT1_WAKEUP_ANY_HIGH; Serial.print("opening"); break;
    case CLOSED: wakeMode = ESP_EXT1_WAKEUP_ALL_LOW; Serial.print("closing"); break;
  }

  Serial.print(" of ");
   
  switch(switchName){
    case DOOR: esp_sleep_enable_ext1_wakeup(DOOR_BUTTON_MASK, wakeMode); Serial.print("DOOR"); break;
    case FLAP: esp_sleep_enable_ext1_wakeup(FLAP_BUTTON_MASK, wakeMode); Serial.print("FLAP"); break;
  }

  Serial.println(".");
}

int voltageReading(){
  pinMode(VOLTAGE_PIN, INPUT);
  
  int voltageValue = 0;
  for(int i=0; i<STABILISING_DATA_POINTS; i++){
    voltageValue = voltageValue + analogRead(VOLTAGE_PIN);
    delay(STABILISING_POINT_INTERVAL);
  }
  
  voltageValue = voltageValue / STABILISING_DATA_POINTS;
  return voltageValue;
}

boolean connectToWifi(){
  String wifiConnectionInfo = "Connecting to WiFi";

  
  if(WiFi.status() == WL_CONNECTED){
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
    
    if(failureCountdown < 0) {
      Serial.println("");
      Serial.println("Failed to connect.");
      return false;
    }
  }
  
  Serial.println("");
  Serial.println("WiFi connected"); 
  return true;
}
