//dependencies
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
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


MFRC522::MIFARE_Key key;
//authentication return status code
MFRC522::StatusCode status;
// Defined pins to module RC522
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

const int ipaddress[4] = {192,168,1,138};

//wifi
#define port 80
#define ssid "Hogar"
#define psw "casacorzini10"

//hardware
#define LED_VERDE 23
#define LED_ROJO 24

//states
#define CLOSE 0
#define OPEN 1
#define REST 3

//variables
const String gate_id = "gate_id";
int ESTADO = REST;
bool isResident = false;
WebServer server(port);

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

  server.on(UriBraces("/openGate/{}/{}"), []() {
    String gateId = server.pathArg(0);
    String token = server.pathArg(1);

    if (gateId == gate_id) { // si es la misma que la que esta definida arriba
      if (valid(token)) {
          isResident = true;
          Serial.println("opening gate...");
          server.send(200, "text/plain", "{err:false, msg:'opening gate...'}");
      }
      else {
        server.send(400, "text/plain", "{err:true, msg:'invalid token'}");
      }
    }
    else{
        server.send(400, "text/plain", "{err:true, msg:'invalid gate_id'}");
    }

  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();

  //implement fingerprint
  
  switch(ESTADO){
    case OPEN:
      openGate();
      isResident = false;
      ESTADO = CLOSE;
    break;
    case CLOSE:
    closeGate();
    ESTADO = REST;
    break;
    case REST:
     if (isResident){
        ESTADO = OPEN;
     }
     else{
       // DO NOTHING
     }
     break;
  }

  //readRFID();
if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  String buffer;
  for (byte byte = 0 ; byte < rfid.uid.size ; byte++)
    buffer += String(rfid.uid.uidByte[byte] < 0x10 ? " 0" : " ") + String(rfid.uid.uidByte[byte], HEX);
  Serial.println("NUID: " + buffer);

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(2);//allow the cpu to switch to other tasks
}

void openGate(){
  Serial.println("");
  Serial.println("<------------>");
  Serial.println("abriendo puerta");
  delay(2000);
  Serial.println("se termino de abrir puerta");
  Serial.println("<------------>");
}

void closeGate(){
  Serial.println("");
  Serial.println("<------------>");
  Serial.println("cerrando puerta");
  delay(2000);
  Serial.println("se termino de cerrar puerta");
  Serial.println("<------------>");
}

bool valid(String token){
  // implement validator
  if (token.length() > 5){
    return true;
  }
  return false;
}

void readRFID(void ) { /* function readRFID */
  ////Read RFID card

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new 1 cards
  if ( ! rfid.PICC_IsNewCardPresent())
  
    return;

  // Verify if the NUID has been readed
  if (  !rfid.PICC_ReadCardSerial())
    return;

  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  Serial.print(F("RFID In dec: "));
  printDec(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

}


/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

