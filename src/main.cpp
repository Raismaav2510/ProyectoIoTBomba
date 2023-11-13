#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal_I2C.h>

const int lcdColumns = 16;
const int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

const int BombaPin = 2;
const int FCPin = 34;

const char* ssid = "Next";  // Tu SSID
const char* password = ":v123456789";  //Tu Clave
WebServer server(80);

float hD = 0;

void handle_NotFound() {
    server.send(404, "text/plain", "La pagina no existe");
}

String SendHTML() {
    // Cabecera de todas las paginas WEB
    String ptr = "<!DOCTYPE html> <html>\n";

    // <meta> viewport. Para que la pagina se vea correctamente en cualquier dispositivo
    ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\", charset=\"UTF-8\">\n";
    ptr += "<title>Control Humedad</title>\n";
    ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
    ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
    ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
    ptr += "p {font-size: 20px;color: #888;margin-bottom: 10px;}\n";
    ptr += "</style>\n";
    ptr += "<script>function refreshPage() { location.reload(); }</script>\n"; // Script de actualización
    ptr += "</head>\n";
    ptr += "<body>\n";
    /*
     * Encabezados de la pagina
     */
    ptr += "<h1>ESP32 WEB Server</h1>\n";

    ptr += "<p>Humedad en tierra: ";
    ptr += hD;
    ptr +="%</p>";

    if (hD > 50) {
        ptr += "<p>Bomba Off</p>";
    } else {
        ptr += "<p>Bomba On</p>";
    }

    ptr += "<script>setTimeout(refreshPage, 1000);</script>\n"; // Actualiza cada 5 segundos
    ptr += "</body>\n";
    ptr += "</html>\n";
    return ptr;
}

void handle_OnConnect() {
    server.send(200, "text/html", SendHTML());
}

void setup() {
    pinMode(BombaPin, OUTPUT);
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();

    Serial.println("DHT11 test!");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        lcd.setCursor(0, 0);
        lcd.print("Buscando red:");
        lcd.setCursor(0, 1);
        lcd.println(ssid);
    }
    lcd.clear();
    Serial.println("");
    Serial.print("Conectado a ");
    Serial.println(ssid);
    Serial.print("Direccion IP: ");
    Serial.println(WiFi.localIP());
    lcd.setCursor(0, 0);
    lcd.print("Direccion IP:");
    lcd.setCursor(0, 1);
    lcd.println(WiFi.localIP());

    server.on("/", handle_OnConnect); // 1
    server.onNotFound(handle_NotFound); // 3

    server.begin();
    Serial.println("Servidor HTTP iniciado");
    delay(5000);
}

void loop() {
    server.handleClient();
    hD = map(analogRead(FCPin), 0, 4095, 100, 0);
    delay(2000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Humedad:");
    lcd.print(hD);
    lcd.print("%");
    lcd.setCursor(0, 1);

    if (hD > 50) {
        digitalWrite(BombaPin, LOW);
        lcd.print("Bomba: Off");
    } else {
        digitalWrite(BombaPin, HIGH);
        lcd.print("Bomba: On");
    }
}
