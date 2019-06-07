#include "setup.h"
#include "Arduino.h"
extern ESP8266WebServer server;
extern IPAddress apIP;

void Setup::server_init() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_STA);
  byte tries = 11;
  WiFi.begin(SSID.c_str(), password.c_str());
  while (--tries && WiFi.status() != WL_CONNECTED)
  {
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
      all_right = true;
      break;
    }
    Serial.print(".");
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED) {
    // Если не удалось подключиться запускаем в режиме AP
    all_right = false;
    Serial.println("");
    Serial.println("WiFi up AP");
    StartAPMode();
  }
}

void Setup::update_OTA() {
  if (all_right) {
  MDNS.begin(host.c_str());
  }
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      WiFiUDP::stopAll();
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });
}

bool Setup::StartAPMode() {
  WiFi.disconnect();
  // Меняем режим на режим точки доступа
  WiFi.mode(WIFI_AP_STA);
  // Задаем настройки сети
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  // Включаем WIFI в режиме точки доступа с именем и паролем
  // хронящихся в переменных _ssidAP _passwordAP
  WiFi.softAP(_ssidAP.c_str(), _passwordAP.c_str());
  return true;
}


/* --------------- SSDP протокол ------------------*/
void Setup::SSDP_init(void) {
  server.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(server.client());
  });
  //Если версия  2.0.0 закаментируйте следующую строчку
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(SSDP_Name);
  SSDP.setSerialNumber("001788102201");
  SSDP.setURL("/");
  SSDP.setModelName("Alarm clock");
  SSDP.setModelNumber("v.1.2");
  SSDP.setModelURL("");
  SSDP.setManufacturer("Nazarii Romanchenko");
  SSDP.setManufacturerURL("");
  SSDP.begin();
}


bool Setup::is_authentified() {
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.startsWith("ESPSESSIONID=1")) {
      Serial.println("Authentification admin Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}



