#include "macros.h"

Servo brushless;
Adafruit_ADS1115 MUX1;
Adafruit_ADS1115 MUX2;

void readSensors(bool o = 0);
void onReceive(int packetSize); 
void sendMessage(String outgoing); 
void setbrushless();

int sensors[8] = {};
int ant = 0;
unsigned long last_send = 0;
byte msgCount = 0;
String message = "";
String incoming = "";


void setup() {
  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA,CS_LORA);
  Wire.begin(SDA_OLED, SCL_OLED);
  Serial.begin(115200);
  setbrushless();
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
  

  if (millis() - last_send >= 0) {
    last_send = millis();
   
    for(byte i = 0; i < 8; i++) {
      message += " ";
      message +=  String(sensors[i]);
      message += " ";
    }
  
    sendMessage(message);
    message = "\0";  
    
  }
  /*
  onReceive(LoRa.parsePacket());
  if(incoming == "MOTOR ACTIVE") {
    brushless.write(MAXSPEED);
  } else {
    brushless.write(MINSPEED);
  }
  Serial.println(incoming);
  */
  
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

void sendMessage(String outgoing) {
  LoRa.beginPacket();               
  LoRa.write(destination);           
  LoRa.write(local);            
  LoRa.write(msgCount);             
  LoRa.write(outgoing.length());     
  LoRa.print(outgoing);                
  LoRa.endPacket();                   
  msgCount++;                      
}
void onReceive(int packetSize) {
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
void setbrushless() {
  brushless.attach(13);
  brushless.write(MINSPEED);
}