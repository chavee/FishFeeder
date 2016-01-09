/*  NETPIE Fish Feeder
 *  Dependencies
 *    - Microgear-esp8266-arduino https://github.com/netpieio/microgear-esp8266-arduino
 *    - WiFiManager https://github.com/tzapu/WiFiManager.git
 *    - ESP8266-OLED-SSD1306 https://github.com/squix78/esp8266-oled-ssd1306
 */

#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <MicroGear.h>
#include "AppKey.h"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <Wire.h>
#include "SSD1306.h"
#include "SSD1306Ui.h"

#define ALIAS           "fishfeeder"
#define ACCESSPOINTNAME "FishFeeder"
#define FRAMECOUNT      1

SSD1306   display(0x3c, D5, D6);
SSD1306Ui ui ( &display );

bool (*frames[FRAMECOUNT])(SSD1306 *display, SSD1306UiState* state, int x, int y);
bool drawAPFrame(SSD1306 *display, SSD1306UiState* state, int x, int y);
bool drawFeedFrame(SSD1306 *display, SSD1306UiState* state, int x, int y);

WiFiClient client;
AuthClient *authclient;

int startfeed, feedtime, foodcount, lastupdate, feedpressed;
char fc[10];

MicroGear microgear(client);

void publishFoodCount() {
  sprintf(fc,"%d",foodcount/1);
  microgear.publish("/foodcount",fc);
}

void startFeed() {
  if (startfeed == 0) {
    digitalWrite(15, HIGH);
    startfeed = millis();
  }
}

void stopFeed() {
  digitalWrite(15, LOW);
  startfeed = 0;
}

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  msg[msglen] = '\0';

  if (strcmp((char *)msg,"start")==0) {
    lastupdate = millis();
    startFeed();
  }
  else if (strcmp((char *)msg,"stop")==0) {
    stopFeed();
  }
  else if (strcmp((char *)msg,"hi")==0) {
    publishFoodCount();
  }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
  publishFoodCount();
}

bool drawFeedFrame(SSD1306 *display, SSD1306UiState* state, int x, int y) {
  if ( microgear.connected()) {    
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_16);
    display->drawString(6, 0,"FOOD COUNT");

    sprintf(fc,"%d",foodcount/1);
    display->setFont(ArialMT_Plain_24);
    display->drawString(56-strlen(fc)*4, 20, fc);

    display->setFont(ArialMT_Plain_10);
    display->drawString(0, 50, "<Feed>");
    display->drawString(90, 50, "<Reset>");
  }
  else {
    display->setFont(ArialMT_Plain_16);
    display->drawString(6 , 20, "Connecting..");
  }
  return false;
}

bool drawAPFrame(SSD1306 *display, SSD1306UiState* state, int x, int y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  display->drawString(0, 0, "Please connect");
  display->drawString(0, 18, "to access point");
  display->drawString(16, 40, ACCESSPOINTNAME);
  return false;
}

void setup() {
    /* Init OLED*/
    ui.setTargetFPS(5);
    ui.setActiveSymbole((const char[]) {0,0,0,0,0,0,0,0});
    ui.setInactiveSymbole((const char[]) {0,0,0,0,0,0,0,0});
    ui.setFrames(frames, FRAMECOUNT);
    ui.init();

    /* Event listener */
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    startfeed = 0;
    feedtime = 0;
    foodcount = 0;
    feedpressed = 0;

    pinMode(4, INPUT);
    pinMode(5, INPUT);
    pinMode(15, OUTPUT);
    digitalWrite(15, LOW);

    Serial.begin(115200);
    Serial.println("Starting...");

    WiFiManager wifiManager;
    wifiManager.setTimeout(60);

    /* Display wifi setup method */
    frames[0] = drawAPFrame;
    ui.update();

    if(!wifiManager.autoConnect(ACCESSPOINTNAME)) {
      Serial.println("Failed to connect and hit timeout");
      delay(3000);
      ESP.reset();
      delay(5000);
    }

    /* Switch to feeding mode */
    frames[0] = drawFeedFrame;    
    ui.update();

    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);
}

void loop() {
    ui.update();

  if (digitalRead(5)==LOW) {
    if (!feedpressed) {
      feedpressed = 1;
      startFeed();
    }
    lastupdate = millis();
  }
  else {
    if (feedpressed) {
      feedpressed = 0;
      stopFeed();
    }
  }

  if (digitalRead(4)==LOW) {
    if (feedtime != 0) {
      feedtime = 0;
      foodcount = 0;
      publishFoodCount();
    }
  }

  if (microgear.connected()) {
    microgear.loop();

    if (startfeed > 0) {
      feedtime += millis() - startfeed;
      startfeed = millis();
  
      if (feedtime/1000 != foodcount) { 
        foodcount = feedtime/1000;
        publishFoodCount();
      }
      if (millis() - lastupdate > 1000) stopFeed();
    }
    
  }
  else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }

}


