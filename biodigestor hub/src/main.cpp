#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#include <addons/RTDBHelper.h>
#include <addons/TokenHelper.h>

#define WIFI_SSID "IFPB"
#define EAP_USERNAME "20201750008" 
#define EAP_PASSWORD "P3dr0_Gu3rr4" 
#define DATABASE_URL "https://biodigestor-lora-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyAOzDmANFl73QsQ3GoFNLTFhs02e_bthd0 "

#define MISO_LORA   19
#define MOSI_LORA   27
#define RST_LORA    14
#define SCK_LORA    5
#define CS_LORA     18
#define DI0_LORA    26
//:p
#define local       0xBB
#define destination 0xFF

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 15, 4, 16);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long last_send = 0;
unsigned long sendDataPrevMillis = 0;
byte msgCount = 0;
int sensors[8];

bool signupOK = false;

String incoming;
String mensagem;
char incomingcopy[150] = {"teste"};

void onReceive(int packetSize);
void sendMessage(String outgoing);
void StringToInt(int *i, char *s);
void upFirebase();
void initialconection();

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA,CS_LORA);
  LoRa.setPins(CS_LORA, RST_LORA, DI0_LORA);
  u8g2.begin();
  
  if(!LoRa.begin(915E6)) 
  {             
    while (1);                      
  }
  initialconection();
}

//strlen(sensors[i])/2



void loop() {
  //u8g2.clearBuffer();
  upFirebase();
  if (millis() - last_send >= 1000) {
    last_send = millis();
    onReceive(LoRa.parsePacket());
    strcpy(incomingcopy, incoming.c_str()); 
    if(incoming != "\n") {
      StringToInt(sensors, incomingcopy); 
    }
  }
  
    //mensagem = "MOTOR ACTIVE";
    //sendMessage(mensagem);


  u8g2.setFont(u8g2_font_5x8_tr);
  

  u8g2.clearBuffer();
  for(byte i = 1; i <= 8; i++) {
    u8g2.drawStr( (128/2)-(4*2.5) ,8 *i, String(sensors[i-1]).c_str());
  }
  u8g2.sendBuffer();
    
  
  //u8g2.sendBuffer();
  incoming = '\0';
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

void StringToInt(int *i, char *s) {
  char * token = strtok(s, " ");
  byte o = 0;

  while( token != NULL) {
    i[o] = atoi(token);
    token = strtok(NULL, " ");
    o++;
  }
}

void upFirebase() {
  unsigned long prev = 0;
  byte i = 0;
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis >= 5000))  {
    sendDataPrevMillis = millis();
      while(i < 8) {
        if(millis() - prev >= 500) {
          prev = millis();
          Firebase.RTDB.set(&fbdo, ("/biodigestor/Sensor" + String(i+1)) , sensors[i]);
          i++;
        }
      }
  }
}

void initialconection() {
  WiFi.begin(WIFI_SSID, WPA2_AUTH_PEAP, EAP_USERNAME, EAP_USERNAME, EAP_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(10);
  } 
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.token_status_callback = tokenStatusCallback;
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.tcp_data_sending_retry = 1;
  config.timeout.socketConnection = 5000;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  fbdo.setBSSLBufferSize(16384, 16384);
  Firebase.reconnectNetwork(true);
  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
}