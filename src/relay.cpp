#include <Arduino.h>

const uint8_t RELAY_PIN = 26;
const int RELAY_CLOSE_IN = LOW;

int initRelay() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, !RELAY_CLOSE_IN);

    return !RELAY_CLOSE_IN;
}

bool isRelayClosed() {
    int relayState = digitalRead(RELAY_PIN);
    
    return relayState == RELAY_CLOSE_IN;
}

void turnOnShopVac() {
  Serial.println("Turning on the shop vac");

  digitalWrite(RELAY_PIN, RELAY_CLOSE_IN);
}

void turnOffShopVac(int d) {
  Serial.println("Turning off the shop vac");
      
  for (int i = d; i > 0; i--) {
    Serial.printf("%d\n", i);
    delay(1000);
  }

  digitalWrite(RELAY_PIN, !RELAY_CLOSE_IN);
}