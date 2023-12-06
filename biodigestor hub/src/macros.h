#ifndef MACROS_H
#define MACROS_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
#include <addons/RTDBHelper.h>
#include <addons/TokenHelper.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define WIFI_SSID "IFPB"
#define EAP_USERNAME "20201750008" 
#define EAP_PASSWORD "P3dr0_Gu3rr4" 
#define DATABASE_URL "https://biodigestor-lora-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyAOzDmANFl73QsQ3GoFNLTFhs02e_bthd0"

#define MISO_LORA   19
#define MOSI_LORA   27
#define RST_LORA    14
#define SCK_LORA    5
#define CS_LORA     18
#define DI0_LORA    26
#define local       0xBB
#define destination 0xFF
#define UpdateTime  0


#endif