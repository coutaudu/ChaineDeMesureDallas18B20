#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define DELAI_ACQUISITION 30000
#define ONEWIRE_PIN 2  // DS18B20 pin
OneWire oneWire(ONEWIRE_PIN);
DallasTemperature my18B20Sensors(&oneWire);
DeviceAddress mySensorsAddresses[2];

WiFiUDP Udp;
const uint16_t portSrc = 12345;
const uint16_t portDest = 32771;
//const char * hostDest = "coutaudu.freeboxos.fr";
const char * hostDest = "88.174.39.26";
char packet[255];


void setup() {
  delay(5000);
  initSerialLine();
  delay(5000);
  Serial.println();
  Serial.println();
  initWifiConnexion("Abricot12","Zt8d#62x@TaX^ef623@K");
  //initWifiConnexion("APUC","ze9tikwms3z8t72");
  initUdpStream(portSrc);
  initDallas18B20Sensor(&my18B20Sensors, mySensorsAddresses);
}

void loop() {
  char* buffer;
  buffer = packet;  
  readFromSensors(&my18B20Sensors);
  
  getDataFromSensor(&my18B20Sensors, mySensorsAddresses, 0, buffer);
  Serial.println(buffer);
  buffer += strlen(buffer);
  getDataFromSensor(&my18B20Sensors, mySensorsAddresses, 1, buffer);
  Serial.println(buffer);
  buffer = packet;
  sendDataUdp(buffer);
  delay(DELAI_ACQUISITION);
}

void sendDataUdp (char* buffer){
  Udp.beginPacket(hostDest, portDest);
  Udp.write(packet);
  Udp.endPacket();
}

void getDataFromSensor(DallasTemperature* sensors, DeviceAddress* sensorsAddresses, unsigned int index, char* buffer) {
  float temperature;
  temperature = sensors->getTempC(sensorsAddresses[index]);
  //sprintf(buffer, "@MAC[%s], @OW[%08X%08X], T[%3.5f], R[%2d],",
  sprintf(buffer, "@MAC, %s, @OW, %08X%08X, T, %3.5f, R, %2d, ",
  WiFi.macAddress().c_str(),
  *((long*) sensorsAddresses[index]),
  *(((long*)sensorsAddresses[index])+1),
  temperature,
  sensors->getResolution(sensorsAddresses[index]));
}

void readFromSensors(DallasTemperature* sensors){
  sensors->requestTemperatures();
//  while (!sensors->isConversionComplete()){Serial.println("Acquisition en cours.");}
  while (!sensors->isConversionComplete());
  return;  
}

int initDallas18B20Sensor(DallasTemperature* sensors, DeviceAddress* sensorsAddresses) {
  int nbCapteurs;
  sensors->begin();
  nbCapteurs = sensors->getDeviceCount();

  Serial.print("Capteurs branchés: ");
  Serial.println(nbCapteurs, DEC);
  for (int i=0; i<nbCapteurs; i++){
    if(!sensors->getAddress(sensorsAddresses[i], i)) {
      Serial.println("Echec a la detection du capteur.");
      return -1;
    }
    Serial.print("Adresse OneWire: [");
    printAddress(sensorsAddresses[i]);
    Serial.println("]");
  }
  sensors->setResolution(sensorsAddresses[0], 12);
  sensors->setResolution(sensorsAddresses[1], 9);
  return 0;
}

void printAddress(DeviceAddress deviceAddress) {
  for(uint8_t i = 0; i < 8; i++) {
    if(deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

int initUdpStream(uint16_t port) {
  Serial.println("initUdpStream");
  Udp.begin(portSrc);
  return 0;
}

int initSerialLine(){
  Serial.begin(115200);
  return 0;
}

int initWifiConnexion(char* SSID, char* PASSWORD){
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connexion Wifi en cours.");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connecté, @IP: [");
  Serial.print(WiFi.localIP());
  Serial.println("]");

  return 0;
  
}
