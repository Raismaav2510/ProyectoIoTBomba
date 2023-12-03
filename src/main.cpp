#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

const int BombaPin = 2;

const char* ssid = "Next";  // Tu SSID
const char* password = ":v123456789";  //Tu Clave

float hD = 0;

void apiConnection() {
    HTTPClient http;
    http.begin("http://192.168.1.110/iot/iotWaterPump.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.GET();

    if (httpCode > 0) {
        Serial.println("HTTP - " + String(httpCode));
        if (httpCode == 200){
            String response = http.getString();
            hD = response.toFloat();
            Serial.println(hD);
        }
    } else {
        Serial.println("HTTP - " + String(httpCode));
    }
    http.end();
}


void setup() {
    pinMode(BombaPin, OUTPUT);
    Serial.begin(115200);

    Serial.println("Bomba RL");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Conectado a ");
    Serial.println(ssid);
    Serial.print("Direccion IP: ");
    Serial.println(WiFi.localIP());

    delay(5000);
}

void loop() {
    apiConnection();
    if (hD > 60) {
        digitalWrite(BombaPin, LOW);
        Serial.println("Bomba: Off");
    } if (hD < 30) {
        digitalWrite(BombaPin, HIGH);
        Serial.println("Bomba: on");
    }
    delay(5000);
}
