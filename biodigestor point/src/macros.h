#ifndef MACROS_H
#define MACROS_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <SPI.h>
#include <LoRa.h>

#define MISO_LORA   19
#define MOSI_LORA   27
#define RST_LORA    14
#define SCK_LORA    5
#define CS_LORA     18
#define DI0_LORA    26

#define SCL_OLED    15
#define SDA_OLED    4
#define RST_OLED    16

#define local       0xFF
#define destination 0xBB

#define MINSPEED 50
#define MAXSPEED 180

#endif