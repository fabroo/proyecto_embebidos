//dependencies
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid

#define SS_PIN    21
#define RST_PIN   22
#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16
#define greenPin     12
#define redPin       32

byte nuidPICC[4] = {0, 0, 0, 0};

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
#define ssid "CASA11"
#define psw "5432154321"

//hardware
#define LED_VERDE 23
#define LED_ROJO 24

//states
#define CLOSE 0
#define OPEN 1
#define SEARCH 2
#define REST 3

//variables
const String gate_id = "gate_id";
int ESTADO = SEARCH;
bool searching = false;
bool isResident = false;
String residentName = "alguien";
WebServer server(port);

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  if(searching){
    Serial.println("Terminó el timer. No se encontró RFID.");
    searching = false;
  }
  
  portEXIT_CRITICAL_ISR(&timerMux);

}

void IRAM_ATTR onTimer2() {
  portENTER_CRITICAL_ISR(&timerMux);
  searching = false;
  Serial.print("RFID encontrado. Bienvenido, ");
  Serial.println(residentName);
  ESTADO = OPEN;
  portEXIT_CRITICAL_ISR(&timerMux);

}

void setup(void) {
  Serial.begin(115200);



  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, psw);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.localIP());


  SPI.begin();
  rfid.PCD_Init();
  delay(2000);

  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
  if (rfid.PCD_PerformSelfTest()) Serial.println("Passed Self-Test");


  server.on(("/"), []() {
    server.send(200, "text/plain", "SERVIDOR EQUIPO PIOLA");
  });

  server.on(UriBraces("/openGate"), HTTP_POST, []() {

    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    if (error) {
      Serial.println("error wacho qsy");
    }
    String rfid1 = doc["rfidList"][0];
    String rfid2 = doc["rfidList"][1];
    String namee = doc["name"];
    residentName = namee;

    // si es la misma que la que esta definida arriba
    Serial.println("Empezando el reconocimiento...");
    searching = true;
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer2, true);
    timerAlarmWrite(timer, 3000000, false);
    timerAlarmEnable(timer);
    ESTADO = SEARCH;
    isResident = true;

    server.send(200, "text/plain", "{err:false, msg:'opening gate...'}");


  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();

  //implement fingerprint

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

      /*if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
        return;

      String buffer;
      for (byte byte = 0 ; byte < rfid.uid.size ; byte++)
        buffer += String(rfid.uid.uidByte[byte] < 0x10 ? " 0" : " ") + String(rfid.uid.uidByte[byte], HEX);
      Serial.println("Nuevo tag: " + buffer);
      Serial.println();

      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();

      if (isResident) {
        ESTADO = OPEN;
      }
      else {
        // DO NOTHING
      }
      break;*/

      timer = timerBegin(1, 80, true);
      timerAttachInterrupt(timer, &onTimer, true);
      timerAlarmWrite(timer, 20000000, false);
      timerAlarmEnable(timer);
  }

  delay(2);
}

void openGate() {

  Serial.println("abriendo puerta");
  delay(2000);
  Serial.println("se termino de abrir puerta");

}

void closeGate() {

  Serial.println("cerrando puerta");
  delay(2000);
  Serial.println("se termino de cerrar puerta");

}

bool valid(String token) {
  // implement validator
  if (token.length() > 5) {
    return true;
  }
  return false;
}