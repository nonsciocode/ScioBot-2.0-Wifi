
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>

// Replace with your network credentials
const char* ssid = "BellaFonte";
const char* password = "farmazabu!";

const int STOP = 0;
const int FORWARD = 1;
const int BACKWARD = 2;
const int PIVOT_LEFT = 3;
const int PIVOT_RIGHT = 4;

int state = 0;
bool didStateChange = false;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


void setNewState(int newState) {

  Serial.print("State changed from: ");
  Serial.print(state);
  Serial.print(" to: ");
  Serial.println(newState);

  state = newState;
  didStateChange = true;


  
}
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(14, OUTPUT);

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/forward", HTTP_POST, [](AsyncWebServerRequest *request){
    setNewState(FORWARD);
  });
  
  // Route to set GPIO to LOW
  server.on("/backward", HTTP_POST, [](AsyncWebServerRequest *request){
    setNewState(BACKWARD);
  });
  
    // Route to set GPIO to LOW
  server.on("/pivotLeft", HTTP_POST, [](AsyncWebServerRequest *request){
    setNewState(PIVOT_LEFT);
  });

    // Route to set GPIO to LOW
  server.on("/pivotRight", HTTP_POST, [](AsyncWebServerRequest *request){
    setNewState(PIVOT_RIGHT);
  });

    // Route to set GPIO to LOW
  server.on("/stop", HTTP_POST, [](AsyncWebServerRequest *request){
    setNewState(STOP);
  });

  // Start server
  server.begin();
}
 
void loop(){

  if(didStateChange) {    
    if(state == FORWARD) {
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(2, HIGH);
      digitalWrite(0, LOW);
      digitalWrite(12, HIGH);
      digitalWrite(14, HIGH);  
      Serial.println("Forward"); 
    } else if(state == BACKWARD) {
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(2, LOW);
      digitalWrite(0, HIGH);
      digitalWrite(12, HIGH);
      digitalWrite(14, HIGH);
      Serial.println("Backward"); 
    } else if(state == STOP) {
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      digitalWrite(2, LOW);
      digitalWrite(0, LOW);
      digitalWrite(12, LOW);
      digitalWrite(14, LOW);
      Serial.println("Stop"); 
    } else if(state == PIVOT_LEFT) {
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(2, HIGH);
      digitalWrite(0, LOW);
      digitalWrite(12, HIGH);
      digitalWrite(14, HIGH);
      Serial.println("Pivot Left"); 
    } else if(state == PIVOT_RIGHT) {
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(2, LOW);
      digitalWrite(0, HIGH);
      digitalWrite(12, HIGH);
      digitalWrite(14, HIGH);
      Serial.println("Pivot Right"); 
    }
    didStateChange = false;
  }
}
