#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MISO_LORA   19
#define MOSI_LORA   27
#define RST_LORA    14
#define SCK_LORA    5
#define CS_LORA     18
#define DI0_LORA    26

#define local       0xBB
#define destination 0xFF

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 15, 4, 16);

unsigned long last_send = 0;
byte msgCount = 0;
int sensors[8];
String incoming;


void onReceive(int packetSize);
void onReceive(int packetSize);
void extrairValoresParaVetor(const char *string, int *vetor, int tamanhoMaximo);

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA,CS_LORA);
  LoRa.setPins(CS_LORA, RST_LORA, DI0_LORA);
  u8g2.begin();
  
  if(!LoRa.begin(915E6)) 
  {             
    while (1);                      
  }
  
}

//strlen(sensors[i])/2



void loop() {
  //u8g2.clearBuffer();
  incoming = "123 456 789";
  if (millis() - last_send >= 0) {
    last_send = millis();
    onReceive(LoRa.parsePacket());

    extrairValoresParaVetor(incoming, sensors, 8);
    Serial.println(sensors[0]);
    Serial.println(sensors[1]);
    Serial.println(sensors[2]);


    //String mensagem = " Ola mundo! :O ";
    //sendMessage(mensagem);
  }
 

  


  u8g2.setFont(u8g2_font_4x6_tr);
  

  u8g2.clearBuffer();
  for(byte i = 1; i <= 8; i++) {
    u8g2.drawStr( (128/2)-(4*2.5) ,6*i,"teste");
  }
  u8g2.sendBuffer();
    
  
  //u8g2.sendBuffer();
  incoming = "\0";
}


void sendMessage(String outgoing) 
{
  LoRa.beginPacket();                   // Inicia o pacote da mensagem
  LoRa.write(destination);              // Adiciona o endereco de destino
  LoRa.write(local);             // Adiciona o endereco do remetente
  LoRa.write(msgCount);                 // Contador da mensagem
  LoRa.write(outgoing.length());        // Tamanho da mensagem em bytes
  LoRa.print(outgoing);                 // Vetor da mensagem 
  LoRa.endPacket();                     
  msgCount++;                           
}
 

void onReceive(int packetSize) 
{
  if (packetSize == 0) return;
  int recipient = LoRa.read();        
  byte sender = LoRa.read();           
  byte incomingMsgId = LoRa.read();     
  byte incomingLength = LoRa.read();    

  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }
}

void extrairValoresParaVetor(const char *string, int *vetor, int tamanhoMaximo) {
    int i = 0;
    int valor;
    while (sscanf(string, "%d", &valor) == 1 && i < tamanhoMaximo) {
        vetor[i] = valor;
        while (*string != ' ' && *string != '\0') {
            string++;
        }
        while (*string == ' ') {
            string++;
        }
        i++;
    }
}