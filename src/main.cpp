#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include "Secrets.h"
#include "HTML.h"

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);

unsigned long lastTime = 0;

String currentIP;

void getIP();
void updateIP();
void handleRoot();
void handleNotFound();
void handleGate();
void handleGarage();

#define GATE_PIN D1
#define GARAGE_PIN D2
#define CLICK_DELAY 500
#define IP_DELAY 1200000 //20 minutes

void setup() {
  Serial.begin(115200); 
	
  wifiMulti.addAP(WIFI_ESSID, WIFI_PASSWORD);

  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Connecting...");
    delay(250);
  }

  Serial.print("Connected at ");
  Serial.println(WiFi.localIP());

  MDNS.begin("gate");
  
  server.on("/", handleRoot);
  server.on("/open/garage", handleGarage);
  server.on("/open/gate", handleGate);
  server.onNotFound(handleNotFound);
  server.begin();
  
  pinMode(GARAGE_PIN, OUTPUT);
  pinMode(GATE_PIN, OUTPUT);
}

void loop() {
  server.handleClient();
  if ((millis() - lastTime) > IP_DELAY) {  
    getIP();
    lastTime = millis();
  }
}

void updateIP() {
  Serial.println("Updating IP...");
  HTTPClient http;
  http.begin("http://" YDNS_USER ":" YDNS_PASSWORD "@ydns.io/api/v1/update/?host=" YDNS_HOST);
  if (http.GET() == HTTP_CODE_OK) {
    Serial.println("IP correctly updated");
  } else
    Serial.println("Error while updating the IP");
  http.end();
}

void getIP() {
  Serial.println("Getting IP...");
  String ip;
  HTTPClient http;
  http.begin("http://ydns.io/api/v1/ip");
  if (http.GET() == HTTP_CODE_OK) { 
    ip = http.getString();
    Serial.print("IP is ");
    Serial.println(ip);
    if (ip != currentIP) {
      currentIP = ip;
      updateIP();
    } else
      Serial.println("No need to update the IP");
  }
  http.end();
}

void handleRoot() {
  if(!server.authenticate(HTTP_USER, HTTP_PASSWORD)) {
    server.requestAuthentication();
    server.send(201, "text/plain", "Unauthorized");
  }
  server.send(200, "text/html", rootSource);
}

void handleGate() {
  if(!server.authenticate(HTTP_USER, HTTP_PASSWORD)) {
    server.requestAuthentication();	
    server.send(201, "text/plain", "Unauthorized");
  }
  server.send(200, "text/plain", "Done");
  digitalWrite(GATE_PIN, HIGH);
  delay(CLICK_DELAY);
  digitalWrite(GATE_PIN, LOW);
}

void handleGarage() {
  if(!server.authenticate(HTTP_USER, HTTP_PASSWORD)) {
    server.requestAuthentication();
    server.send(201, "text/plain", "Unauthorized");
  }
  server.send(200, "text/plain", "Done");
  digitalWrite(GARAGE_PIN, HIGH);
  delay(CLICK_DELAY);
  digitalWrite(GARAGE_PIN, LOW);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}
