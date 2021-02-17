/** Copyright (C) 2021  Nonscio, LLC
 *  This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>

//================================================================================
//CONSTANTS
//================================================================================

const char* ssid = "yourssid";
const char* password = "yournetworkpassword";

const int STOP = 0;
const int FORWARD = 1;
const int BACKWARD = 2;
const int PIVOT_LEFT = 3;
const int PIVOT_RIGHT = 4;

//================================================================================
//VARIABLES
//================================================================================

int state = 0;
bool didStateChange = false;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//================================================================================
//FUNCTIONS
//================================================================================

/**
 * Function: setNewState()
 * Description: The setNewState() function 
 */
void setNewState(int newState) {
  
  Serial.print("State changed from: ");
  Serial.print(state);
  Serial.print(" to: ");
  Serial.println(newState);

  state = newState;
  didStateChange = true;
  
}

/**
 * Function: setup()
 * Description: The setup() function runs once after the microcontroller is 
 * booted or reset. Sets the pin modes of the defined motor logic pin to OUTPUT
 * mode.
 */
 
void setup() {
  
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

/**
 * Function: loop()
 * Description: The loop() function runs repeatedly in a loop after the setup()
 * function has completed.
 */
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
