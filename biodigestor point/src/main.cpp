#include "macros.h"

Servo brushless;
Adafruit_ADS1115 MUX1;
Adafruit_ADS1115 MUX2;

void readSensors(bool o = 0);
void onReceive(int packetSize); 
void sendMessage(String outgoing); 
void setbrushless();
void treatincoming();
void createMessage();
void brushlesscontrol();

int sensors[8] = {};
int ant = 0;
byte msgCount = 0;
String message = "";
String incoming = "";
bool alternating = 0;
bool incomingstate = 0;
bool lastincomingstate = 0;
unsigned long timer = 0;

void setup() {
  Serial.begin(115200);
  pinMode(25, OUTPUT);   
  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA,CS_LORA);
  LoRa.setPins(CS_LORA, RST_LORA, DI0_LORA);
  Wire.begin(SDA_OLED, SCL_OLED);
  MUX1.begin(0x48);
  MUX2.begin(0x49);
  setbrushless();
  if(!LoRa.begin(915E6))  while (1);    
}

void loop() {
  readSensors();
  if(millis() - timer >= 250) {
    createMessage();
    sendMessage(message);
    timer = millis();
    Serial.println(message);
    message = "\0";
  }

  onReceive(LoRa.parsePacket());
  treatincoming();
  brushlesscontrol(); 
  incoming = "\0";
}

void brushlesscontrol() {
  digitalWrite(25, incomingstate); 
  if(incomingstate == 1) {
    brushless.write(MAXSPEED);
  } else {
    brushless.write(MINSPEED);
  }
}
void createMessage() {
  for(byte i = 0; i < 8; i++) {
    message += " ";
    message +=  String(sensors[i]);
    message += " ";
  }
}
void treatincoming() {
  if(incoming == "1") {
    incomingstate = 1;
  }
  else if(incoming == "0") {
    incomingstate = 0;
  }
  else {
    incomingstate = lastincomingstate;
  }
  lastincomingstate = incomingstate;
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
    int recipient = LoRa.read();        
    byte sender = LoRa.read();           
    byte incomingMsgId = LoRa.read();     
    byte incomingLength = LoRa.read();    
    while (LoRa.available())  incoming += (char)LoRa.read();
  
}
void setbrushless() {
  brushless.attach(13);
  brushless.write(MINSPEED);
}

