//dependencies
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

//wifi
#define port 80
#define ssid "ssid"
#define psw "password"

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
