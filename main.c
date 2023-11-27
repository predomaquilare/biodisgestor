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

Servo brushless;
Adafruit_ADS1115 MUX1;
Adafruit_ADS1115 MUX2;

void readSensors(bool o = 0);

int sensors[8] = {};
unsigned long last_send = 0;
byte msgCount = 0;

void setup() {
  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA,CS_LORA);
  Wire.begin(SDA_OLED, SCL_OLED);
  Serial.begin(115200);
  
  brushless.attach(32);
  LoRa.setPins(CS_LORA, RST_LORA, DI0_LORA);
  MUX1.begin(0x48);
  MUX2.begin(0x49);

  if(!LoRa.begin(915E6)) 
  {             
    while (1);                      
  }
}

void loop() {
  readSensors();
  brushless.write(0);
  delay(10);
  

  if (millis() - last_send > 10000) {
    last_send = millis();
    String mensagem = " Ola mundo! :O ";
    sendMessage(mensagem);
 
  }
 

  onReceive(LoRa.parsePacket());
}

void readSensors(bool o) {
  for(byte i = 0; i < 8; i++) { 
    if(i < 4) {
      sensors[i] = MUX1.readADC_SingleEnded(i);
    } 
    else {
      sensors[i] = MUX2.readADC_SingleEnded(i-4);
    }
    if(o == 1) {sensors[i] = map(sensors[i], 12800, 5200, 0, 100);}
  }
}

void sendMessage(String outgoing) 
{
  LoRa.beginPacket();                   // Inicia o pacote da mensagem
  LoRa.write(destination);              // Adiciona o endereco de destino
  LoRa.write(local);             // Adiciona o endereco do remetente
  LoRa.write(msgCount);                 // Contador da mensagem
  LoRa.write(outgoing.length());        // Tamanho da mensagem em bytes
  LoRa.print(outgoing);                 // Vetor da mensagem 
  LoRa.endPacket();                     // Finaliza o pacote e envia
  msgCount++;                           // Contador do numero de mensagnes enviadas
}
 

void onReceive(int packetSize) 
{
  if (packetSize == 0) return;          // Se nenhuma mesnagem foi recebida, retorna nada
  int recipient = LoRa.read();          // Endereco de quem ta recebendo
  byte sender = LoRa.read();            // Endereco do remetente
  byte incomingMsgId = LoRa.read();     // Mensagem
  byte incomingLength = LoRa.read();    // Tamanho da mensagem
 
  String incoming = "";
 
  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }
}
