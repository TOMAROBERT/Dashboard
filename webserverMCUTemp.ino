//theiotprojects.com
#include <Ultrasonic.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

// D1 pentru Trig si D2 pentru Echo
Ultrasonic ultrasonic (D0, D1);

#define tempPin 4 
OneWire temp1(tempPin); 
DallasTemperature SenzorTemp(&temp1);
float Celsius=0;

// Date de logare wi-fi personal
const char* ssid = "Tenda_6CFA70";
const char* password = "jokecold781";

// Creare server web asincron pe portul 80
AsyncWebServer server(80);

String getDistance() {
  SenzorTemp.requestTemperatures(); 
  float Celsius=SenzorTemp.getTempCByIndex(0);
  
  if (isnan(Celsius)) {
    Serial.println("Eroare citire date din HC-SR04 !");
    return "";
  }
  else {
    Serial.println(Celsius);
    return String(Celsius);
  }
}
void setup () {
  // Initializare cu baud 115200 bps pentru transmisia datelor
  Serial.begin (115200);
  SenzorTemp.begin(); 

  // Initializare SPIFFS (pentru comunicarea cu fisierele externe)
  if (! SPIFFS.begin ()) {
    Serial.println ("Eroare la initializarea SPIFFS");
    return;
  }

  // Conectare la wi-fi personal
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectare la WiFi...");
  }

  // Afisare adresa IP locala , pentru a accesa pagina Web
  Serial.println(WiFi.localIP());

  // Alocare URL pentru pagina web
  server.on ("/", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request-> send (SPIFFS, "/index.html");
  });
  server.on ("/distance", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request-> send_P (200, "text / plain", getDistance(). c_str ());
  });

  // start server
  server.begin ();
}
void loop() {
}
