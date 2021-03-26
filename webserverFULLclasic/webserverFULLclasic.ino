//theiotprojects.com
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <Ultrasonic.h>

#define tempPin 4 // D2 Node
OneWire temp1(tempPin); 
DallasTemperature SenzorTemp(&temp1);

int initializareApa = 0 ; 
#define apaAnalogInt 14 // D5 Node

// D1 pentru Trig si D2 pentru Echo
Ultrasonic ultrasonic (D0, D1);

// Date de logare wi-fi personal
const char* ssid = "Tenda_6CFA70";
const char* password = "jokecold781";

// Creare server web asincron pe portul 80
AsyncWebServer server(80);

String getDistanta() {
  // Citim distanta din senzorul ultrasonic
  float d = ultrasonic.read();
  // isnan = NaN = date nedefinite , opus pentru read
  if (isnan(d)) {
    Serial.println("Eroare citire date din HC-SR04 !");
    return "";
  }
  else {
    Serial.print("Distanta este:");
    Serial.println(d);
    return String(d);
  }
}

String getTemperatura() {
  SenzorTemp.setWaitForConversion(false);
  SenzorTemp.setResolution(12);
  SenzorTemp.requestTemperatures(); 
  delay(1000);
  float Celsius=SenzorTemp.getTempCByIndex(0);

  if (isnan(Celsius)) {
    Serial.println("Eroare citire date din senzor temperatura !");
    return "";
  }
  else {
    Serial.print("Temperatura este:");
    Serial.println(Celsius);
    Serial.println("*********");
    return String(Celsius);
  }
}

String getNivel() {
    initializareApa = analogRead(apaAnalogInt);
    if (initializareApa <= 100 ){
    Serial.println("Senzorul are nivelul apei : Gol ");  
    return "GOL" ;
    delay(1000);
  }
  else if (initializareApa>330){
    Serial.println("Senzorul are nivelul apei : Ridicat ");
    return "RIDICAT";
    delay(1000);
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

  //Preluarea valorilor din functile get de mai sus
  server.on ("/temperatura", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request-> send_P (200, "text / plain", getTemperatura(). c_str ());
  });
  server.on ("/nivel", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request-> send_P (200, "text / plain", getNivel(). c_str ());
  });
  server.on ("/distanta", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request-> send_P (200, "text / plain", getDistanta(). c_str ());
  });

  // start server
  server.begin ();
}
void loop() {
}
