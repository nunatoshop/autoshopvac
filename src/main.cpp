#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include <wifi_credentials.h>
#include <acs712.h>
#include <button.h>
#include <relay.h>

#define ONBOARD_LED  2

// WIFI
const char* hostname = "Shopvac";

// SERVER
AsyncWebServer server(80);

/**
 * @brief Inits the server.
 * 
 */
void initServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am the Shopvac.");
  });

  server.on("/ping", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send( 200, "text/plain", "pong");
  });

   server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request) {
    int value = GetValue();
    float voltage = GetVoltage();
    float startingVoltage = GetStartingVoltage();
    char json[64];
    sprintf(json, "{\"value\":%d,\"voltage\":%.2f,\"starting_voltage\":%.2f}", value, voltage, startingVoltage);
    request->send( 200, "application/json", json);
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
}

/**
 * @brief Inits the sensors.
 * 
 */
void initSensors() {
  pinMode(ONBOARD_LED,OUTPUT);

  InitButton();
  InitRelay();
}

/**
 * @brief Inits the WiFi and then monitor its status and if for some reason is lost, it will try to reconnected.
 * 
 * @param parameter 
 */
void keepWiFiAlive(void * parameter) {
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname);

  const char* ssid = GetWifiSsid();
  const char* password = GetWifiPassword();
  Serial.printf("Connecting to %s with password %s", ssid, password);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(ONBOARD_LED, HIGH);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  initServer();

  for(;;) {
    if (WiFi.status() != WL_CONNECTED) {
      digitalWrite(ONBOARD_LED, LOW);
      Serial.print("Reconnecting to WiFi");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      digitalWrite(ONBOARD_LED, HIGH);
    }
    delay(5000);
  }
}

/**
 * Setup function.
 */
void setup() {
  Serial.begin(115200);

  initSensors();
  
  // Task to init and monitoring the WiFi connection.
  xTaskCreatePinnedToCore(keepWiFiAlive, "keepWiFiAlive", 10000, NULL, 0, NULL, 0);            
}

/**
 * Loop function.
 * 
 */
void loop() {
  if (IsButtonActive()) {
    DoButtonAction();
  } else {
    CheckConnectedTool();
  }
}