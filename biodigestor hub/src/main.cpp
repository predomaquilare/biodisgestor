#include "macros.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", -3*3600, 60000);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 15, 4, 16);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

byte msgCount = 0;
int sensors[8];
byte timer = 0;
bool signupOK = false;
bool once = 1;

String incoming = "";
String message = "";
bool alternating = 0;
char incomingcopy[130] = {""};

void onReceive(int packetSize);
void sendMessage(String outgoing);
void StringToInt(int *i, char *s);
void upFirebase();
void initialconection();
void showSensors();
void initialtiming();
void checkMotor();

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA,CS_LORA);
  LoRa.setPins(CS_LORA, RST_LORA, DI0_LORA);
  u8g2.begin();
  if(!LoRa.begin(915E6)) while (1);  
  initialconection();
}

void loop() {
  timeClient.update();
  
  if(millis() - timer >= 250) {
    checkMotor();
    sendMessage(message);
    timer = millis();
    message = "\0";
  }

  onReceive(LoRa.parsePacket());

  strcpy(incomingcopy, incoming.c_str()); 
  if(incoming != "\n")  StringToInt(sensors, incomingcopy); 
  upFirebase();
  showSensors();
  incoming = "\0";
}

void initialtiming() {
  timeClient.begin();
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
  while (LoRa.available())  incoming += (char)LoRa.read();
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
  byte prev = 0;
  byte i = 0;
  if (Firebase.ready() && signupOK && (timeClient.getMinutes() == UpdateTime) && (timeClient.getSeconds() == 0) && once == 1)  {
    Firebase.RTDB.pushAsync(&fbdo, "/"+String(timeClient.getDay()+3)+"/"+String(timeClient.getHours()),0);                                  
      while(i < 8) {
        if(millis() - prev >= 254) {
          prev = millis();
          Firebase.RTDB.set(&fbdo, ("/"+String(timeClient.getDay()+3)+"/"+String(timeClient.getHours())+"/Sensor" + String(i+1)) , sensors[i]);   
          i++;
        }
      }
      once = 0;
  }
  if(timeClient.getMinutes() == UpdateTime && timeClient.getSeconds() > 1) once = 1;
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
void showSensors() {
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.clearBuffer();
  for(byte i = 1; i <= 8; i++) {
    u8g2.drawStr( (128/2)-(4*2.5) ,8 *i, String(sensors[i-1]).c_str());
  }
  u8g2.sendBuffer();
  incoming = '\0';
}
void checkMotor() {
  if(Firebase.RTDB.getBool(&fbdo, "/Motor")) {
    if(fbdo.dataType() == "boolean") {
      message += String(fbdo.boolData());
      
    }
  }
}