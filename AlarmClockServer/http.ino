void handleRoot() {
  if (!server.authenticate(start.admin_login.c_str(), start.admin_password.c_str()))
  { return server.requestAuthentication(DIGEST_AUTH, FailedAuth, start.authFailResponse);
  }
  else {
    Serial.println("Enter handleRoot");
    String header;
    if (!start.is_authentified()) {
      server.sendHeader("Location", "/login");
      server.sendHeader("Cache-Control", "no-cache");
      server.send(301);
      return;
    }
    server.send(200, "text/html", RedirectToIndex);
  }
}

void handleNotFound() {
  server.send(404, "text/html", nofound);
}

// Перезагрузка модуля по запросу вида http://192.168.0.132/restart?device=ok
void handle_Restart() {
  if (!server.authenticate(start.admin_login.c_str(), start.admin_password.c_str()))
  { return server.requestAuthentication(DIGEST_AUTH, FailedAuth, start.authFailResponse);
  }
  else {
    String restart = server.arg("device");
    if (restart == "ok") {
      server.send(200, "text / plain", "Reset OK");
      ESP.restart();
    }
    else {
      server.send(200, "text / plain", "No Reset");
    }
  }
}

void handle_start_AP() {
  WiFi.mode(WIFI_AP_STA);
  // Задаем настройки сети
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  // Включаем WIFI в режиме точки доступа с именем и паролем
  // хронящихся в переменных _ssidAP _passwordAP
  WiFi.softAP(start._ssidAP.c_str(), start._passwordAP.c_str());
  server.send(200, "text/plain", "OK");
}

void handle_IntSetJSON() {
  String root = "{}";
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(root);
  json["timezone"] = timezone;
  json["rele"] = rele;
  json["time"] = CurrentTime();
  json["date"] = CurrentDate();
  json["shift"] = shift;
  json["CurAlarmClockSet"] = CurAlarmClockSet;
  root = "";
  json.printTo(root);
  server.send(200, "text/json", root);
}

void handle_ConfigJSON() {
  String root = "{}";
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(root);
  json["SSDP"] = start.SSDP_Name;
  json["ssidAP"] = start._ssidAP;
  json["passwordAP"] = start._passwordAP;
  json["ssid"] = start.SSID;
  json["password"] = start.password;;
  json["ip"] = WiFi.localIP().toString();
  root = "";
  json.printTo(root);
  server.send(200, "text/json", root);
}

void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == start.admin_login &&  server.arg("PASSWORD") == start.admin_password ) {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;
    }

    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }
  server.send(200, "text/html", RedirectToAuthPage);
}

// Функции API-Set
// Установка SSDP имени по запросу вида http://192.168.0.101/ssdp?ssdp=proba
void handle_Set_Ssdp() {
  start.SSDP_Name = server.arg("ssdp"); // Получаем значение ssdp из запроса сохраняем в глобальной переменной
  saveConfig();                 // Функция сохранения данных во Flash пока пустая
  server.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
}

// Установка параметров для подключения к внешней AP по запросу вида http://192.168.0.101/ssid?ssid=home2&password=12345678
void handle_Set_Ssid() {
  start.SSID = server.arg("ssid");            // Получаем значение ssid из запроса сохраняем в глобальной переменной
  start.password = server.arg("password");    // Получаем значение password из запроса сохраняем в глобальной переменной
  saveConfig();                        // Функция сохранения данных во Flash пока пустая
  server.send(200, "text/plain", "OK");   // отправляем ответ о выполнении
}

//Установка параметров внутренней точки доступа по запросу вида http://192.168.0.101/ssidap?ssidAP=home1&passwordAP=8765439
void handle_Set_Ssidap() {              //
  start._ssidAP = server.arg("ssidAP");         // Получаем значение ssidAP из запроса сохраняем в глобальной переменной
  start._passwordAP = server.arg("passwordAP"); // Получаем значение passwordAP из запроса сохраняем в глобальной переменной
  saveConfig();                         // Функция сохранения данных во Flash пока пустая
  server.send(200, "text/plain", "OK");   // отправляем ответ о выполнении
}


// Загрузка данных сохраненных в файл  config.json
bool loadConfig() {
  // Открываем файл для чтения
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    // если файл не найден
    Serial.println("Failed to open config file");
    //  Создаем файл запиав в него аные по умолчанию
    saveConfig();
    return false;
  }
  // Проверяем размер файла, будем использовать файл размером меньше 1024 байта
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }
  // загружаем файл конфигурации в глобальную переменную
  jsonConfig = configFile.readString();
  // Резервируем памяь для json обекта буфер может рости по мере необходимти предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  //  строку возьмем из глобальной переменной String jsonConfig
  JsonObject& root = jsonBuffer.parseObject(jsonConfig);
  // Теперь можно получить значения из root
  start._ssidAP = root["ssidAPName"].as<String>(); // Так получаем строку
  start._passwordAP = root["ssidAPPassword"].as<String>();
  start.SSDP_Name = root["SSDPName"].as<String>();
  start.SSID = root["ssidName"].as<String>();
  start.password = root["ssidPassword"].as<String>();
  return true;
}

bool saveIntSet() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  json["timezone"] = timezone;
  json["rele"] = rele;
  json["time"] = CurrentTime();
  json["date"] = CurrentDate();
  json["shift"] = shift;
  json["CurAlarmClockSet"] = CurAlarmClockSet;
  json.printTo(jsonConfig);
  File configFile = SPIFFS.open("/InteractiveSetting.json", "w");
  if (!configFile) {
    return false;
  }
  json.printTo(configFile);
  return true;
}

// Запись данных в файл config.json
bool saveConfig() {
  // Резервируем памяь для json обекта буфер может рости по мере необходимти предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  // Заполняем поля json
  json["SSDPName"] = start.SSDP_Name;
  json["ssidAPName"] = start._ssidAP;
  json["ssidAPPassword"] = start._passwordAP;
  json["ssidName"] = start.SSID;
  json["ssidPassword"] = start.password;
  json["rele"] = rele;
  json["time"] = CurrentTime();
  json["date"] = CurrentDate();
  // Помещаем созданный json в глобальную переменную json.printTo(jsonConfig);
  json.printTo(jsonConfig);
  // Открываем файл для записи
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    return false;
  }
  json.printTo(configFile);
  return true;
}

void handle_Rele() { // http://192.168.0.132/device?rele=1
  rele = server.arg("rele");
  saveIntSet();
  if (rele == "1") {
    Serial.println("rele_on");
  }
  else if (rele == "0") {
    Serial.println("rele_off");
  }
  server.send(200, "text/plain", "OK");
}

void handleIndex() {
  if (CheckAuth()) {
    Serial.println("CheckAuth true");
    return server.send(200, "text/html", RedirectToIndex);
  }
  else
    server.send(200, "text/html", RedirectToAuthPage);
}





