#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <Ultrasonic.h>
#include <Wire.h>

#define tempPin 4 // D2 Node
OneWire temp1(tempPin); 
DallasTemperature SenzorTemp(&temp1);
int x = 1;

int initializareApa = 0 ; 
#define apaAnalogInt 14 // D5 Node


// D1 pentru Trig si D2 pentru Echo
Ultrasonic ultrasonic (D0, D1);


// D6 pentru pompa DC
#define pompa 12

// Date de logare wi-fi personal
const char* ssid = "Tenda_6CFA70";
const char* password = "jokecold781";

// Creare server web asincron pe portul 80
AsyncWebServer server(80);

String getDistanta() {
  if(x==1){
    // Citim distanta din senzorul ultrasonic
    float d = ultrasonic.read();
    // isnan = NaN = date nedefinite , opus pentru read
   
    if (d<10) {
      Serial.print("Distanta mai mica de 10cm : ");
      Serial.println("Pompa Activa!");
      digitalWrite(pompa, LOW); //Activare pompa 
      return String(d);
    }
    else {
      Serial.print("Distanta mai mare de 10cm : ");
      Serial.println(d);
      digitalWrite(pompa, HIGH); //Dezactivare pompa
      return String(d);
    }
  }else if(x==0){
      Serial.println("Senzorul ultrasonic este OPRIT");
      return "0";
  }
}

String getTemperatura() {
  if(x==1){
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
  }else if(x==0){
     Serial.println("Senzorul de temperatura este OPRIT");
     return "0";
  }
}

String getNivel() {
  if(x==1){
    initializareApa = analogRead(apaAnalogInt);
      if (initializareApa <= 100 ){
      Serial.println("Senzorul are nivelul apei : Gol ");  
      return "54" ;
      delay(1000);
    }
    else if (initializareApa>330){
      Serial.println("Senzorul are nivelul apei : Ridicat ");
      return "90";
      delay(1000);
    }
  }else if(x==0){
    Serial.println("Senzorul de nivel este OPRIT");
    return "0";
  }
}

void setup () {
  // Initializare cu baud 115200 bps pentru transmisia datelor
  Serial.begin (115200);
  SenzorTemp.begin(); 
  pinMode(pompa, OUTPUT);
  digitalWrite(pompa, HIGH);


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
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/styles.css", "text/css");
  });

  // ON / OFF pentru senzori , folosind butoanele 
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    x = 1;
    request->send(SPIFFS, "/index.html", String(), false);
  });
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    x = 0;
    request->send(SPIFFS, "/index.html", String(), false);
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
