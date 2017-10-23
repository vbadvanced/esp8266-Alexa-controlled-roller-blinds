#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "Switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include "SSID_PASSWORD.h"
#include "rollerBlinds.h"

// prototypes
boolean connectWifi();

//on/off callbacks 
void officeLightsOn();
void officeLightsOff();
void kitchenLightsOn();
void kitchenLightsOff();

// Change this before you flash
const char* ssid = SSID;
const char* password = PASSWORD;

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *office = NULL;
Switch *kitchen = NULL;
RollerBlinds *rollerBlinds1 = 0;
RollerBlinds *rollerBlinds2 = 0;

void setup()
{
  Serial.begin(115200);
  delay(100);
   
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected) {
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    office = new Switch("blinds", 80, officeLightsOn, officeLightsOff);
    kitchen = new Switch("kitchen lights", 81, kitchenLightsOn, kitchenLightsOff);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*office);
    upnpBroadcastResponder.addDevice(*kitchen);
  }
  else {
    Serial.println("Cannot connect to wifi. Restarting...");
    delay(100);
    ESP.restart();
  }
  rollerBlinds1 = new RollerBlinds(16, 5, 4, 0);
  rollerBlinds2 = 0;//new RollerBlinds(16, 5, 4, 0);
}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      kitchen->serverLoop();
      office->serverLoop();
	 }
   rollerBlinds1->loop();
}

void officeLightsOn() {
    Serial.print("Switch 1 turn on ...");
    rollerBlinds1->goDown();
}

void officeLightsOff() {
    Serial.print("Switch 1 turn off ...");
    rollerBlinds1->goUp();
}

void kitchenLightsOn() {
    Serial.print("Switch 2 turn on ...");
}

void kitchenLightsOff() {
  Serial.print("Switch 2 turn off ...");
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}
