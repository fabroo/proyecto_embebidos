//dependencies
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>
#include <ESP32Servo.h>


#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid

#define SS_PIN    21
#define RST_PIN   22
#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16
#define VERDE     15
#define ROJO       5
#define servoPin 4

byte nuidPICC[4] = {0, 0, 0, 0};
Servo puerta;

volatile int interruptCounter;
int totalInterruptCounter;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

MFRC522::MIFARE_Key key;
//authentication return status code
MFRC522::StatusCode status;
// Defined pins to module RC522
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

const int ipaddress[4] = {192, 168, 1, 138};

//wifi
#define port 80
#define ssid "Hogar"
#define psw "casacorzini10"


//states
#define CLOSE 0
#define OPEN 1
#define SEARCH 2
#define REST 3

//variables
const String gate_id = "gate_id";
int ESTADO = REST;
bool searching = false;
bool isResident = false;
String residentName = "alguien";
WebServer server(port);

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  if(searching){
    Serial.println("Terminó el timer. No se encontró RFID.");
    searching = false;
    ESTADO = REST;
  }
  
  portEXIT_CRITICAL_ISR(&timerMux);

}

void setup(void) {
  Serial.begin(115200);
  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  puerta.setPeriodHertz(50);
  puerta.attach(servoPin, 500, 2400);

  pinMode(ROJO, OUTPUT);
  pinMode(VERDE, OUTPUT);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, psw);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.localIP());

  SPI.begin();
  rfid.PCD_Init();

  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
  if (rfid.PCD_PerformSelfTest()) Serial.println("Passed Self-Test");


  server.on(("/"), []() {
    server.send(200, "text/plain", "SERVIDOR EQUIPO PIOLA");
  });

  server.on(UriBraces("/openGate"), HTTP_POST, []() {

    if(ESTADO == REST) {
      StaticJsonDocument<300> doc;
      DeserializationError error = deserializeJson(doc, server.arg("plain"));
      if (error) {
        Serial.println("[ERROR OPEN GATE]");
      }
      String rfid1 = doc["rfidList"][0];
      String rfid2 = doc["rfidList"][1];
      String namee = doc["name"];
      residentName = namee;
  
      // si es la misma que la que esta definida arriba
      Serial.println("Empezando el reconocimiento de RFID...");
      searching = true;
      timer = timerBegin(0, 80, true);
      timerAttachInterrupt(timer, &onTimer, true);
      timerAlarmWrite(timer, 15000000, false);
      timerAlarmEnable(timer);
      ESTADO = SEARCH;
      isResident = true;
  
      server.send(200, "text/plain", "Arrancando a reconocer.");
    } else {
      server.send(400, "text/plain", "Ya se está reconociendo bro.");
    }
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();

  switch (ESTADO) {
    case OPEN:
      
      openGate();
      isResident = false;
      ESTADO = CLOSE;
      break;
    case CLOSE:   
      closeGate();
      ESTADO = REST;
      break;
    case SEARCH:
      
      if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
        return;

      String buffer;
      for (byte byte = 0 ; byte < rfid.uid.size ; byte++)
        buffer += String(rfid.uid.uidByte[byte] < 0x10 ? " 0" : " ") + String(rfid.uid.uidByte[byte], HEX);
      
      Serial.println("Nuevo tag: " + buffer);

      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
      //cambiar el if a que busque un buffer concreto
      if (isResident && buffer == buffer) {
        searching = false;
        Serial.print("RFID encontrado. Bienvenido, ");
        Serial.println(residentName);
        ESTADO = OPEN;
      }
      else {
        // DO NOTHING
      }
      break;
  }
  delay(2);
}

void openGate() {
  Serial.println("opening,.,.,");
  digitalWrite(VERDE, HIGH);
  digitalWrite(ROJO, LOW);
  for (int pos = 0; pos <= 180; pos += 1) {
    puerta.write(pos);
    delay(35);
  }
  delay(500);
  digitalWrite(VERDE, LOW);
}

void closeGate() {
  Serial.println("closing,.,.,");
  digitalWrite(VERDE, LOW);
  digitalWrite(ROJO, HIGH);
    for (int pos = 180; pos >= 0; pos -= 1) {
    puerta.write(pos);
    delay(35);
  }
  delay(500);
  digitalWrite(ROJO, LOW);

}

bool valid(String token) {
  // implement validator
  if (token.length() > 5) {
    return true;
  }
  return false;
}
