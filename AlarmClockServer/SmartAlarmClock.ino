#include "setup.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "index.h" //Our HTML webpage contents
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include "NTPClient.h"
WiFiClient client;
String jsonConfig = "{}";
IPAddress apIP(192, 168, 4, 1);
String rele;
WiFiUDP ntpUDP;
ESP8266WebServer server(80);
Setup start;
File fsUploadFile;
NTPClient timeClient(ntpUDP, "ntp.time.in.ua", 3600, 60000);

boolean access_root;
const char* FailedAuth = "Failed authentification";


int timezone = 2;               // часовой пояс GTM

void setup() {
  HTTP_init();
  start.update_OTA();
  FS_init();
  loadConfig();
  start.server_init();
  if (start.all_right) {
    start.SSDP_init();
  }
  else {
    Serial.println("AP");
  }
  Serial.println(WiFi.localIP());
  server.begin();
  CheckAuth();
}
unsigned long lastMillis;
void loop() {
  server.handleClient();
  delay(1);
  if (millis() - lastMillis > 600000)
  {
    timeClient.update();
    Serial.println(timeClient.getFormattedTime());
    lastMillis = millis();
  }
}

void HTTP_init() {
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.onNotFound(handleNotFound); // Сообщение если нет страницы. Попробуйте ввести http://192.168.0.101/restar?device=ok&test=1&led=on
  server.on("/login", handleLogin);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works without need of authentification");
  });
  server.on("/", handleRoot);
 // server.on("/index.htm", handleIndex);
  server.on("/configs.json", handle_ConfigJSON);
  server.on("/InteractiveSetting.json", handle_IntSetJSON);
  server.on("/ssdp", handle_Set_Ssdp);     // Установить имя SSDP устройства по запросу вида /ssdp?ssdp=proba
  server.on("/ssid", handle_Set_Ssid);     // Установить имя и пароль роутера по запросу вида /ssid?ssid=home2&password=12345678
  server.on("/ssidap", handle_Set_Ssidap); // Установить имя и пароль для точки доступа по запросу вида /ssidap?ssidAP=home1&passwordAP=8765439
  server.on("/restart", handle_Restart);   // Перезагрузка модуля по запросу вида /restart?device=ok
  server.on("/startAP", handle_start_AP); // Вкл AP запросу вида http://192.168.0.101/restart?device=ok
  server.on("/AlarmTime", handle_AlarmTime); // Вкл AP запросу вида http://192.168.0.101/AlarmTime?
  server.on("/device", handle_Rele);
  server.on("/Time", handle_Time);     // Синхронизировать время устройства по запросу вида /Time
  server.on("/TimeZone", handle_time_zone);    // Установка времянной зоны по запросу вида http://192.168.0.101/TimeZone?timezone=3
  server.on("/ShiftSet", handle_ShiftSet);
}

bool CheckAuth() {
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie == "ESPSESSIONID=1") {
      server.send(200, "text/html", RedirectToIndex);
      return true;
    }
  }
    server.send(200, "text/html", RedirectToAuthPage);
    return false;
  
}

