#ifndef SETUP_H
#define SETUP_H

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>        //Содержится в пакете

class Setup {

  public:
    boolean all_right;
    String _response = "";
    void server_init();
    void update_OTA();
    void available();
    bool StartAPMode();
    void SSDP_init(void);

    String SSID = "kv30";
    String password = "29452945";
    String host = "meteoupdate";
    String admin_login = "admin";
    String admin_password = "admin";

    String SSDP_Name = "MeteoStation"; // Имя SSDP
    String _ssidAP = "AlarmClock";   // SSID AP точки доступа
    String _passwordAP = "11111111"; // пароль точки доступа
    String authFailResponse = "Authentication Failed";
    bool is_authentified();
  private:
    String waitResponse_Meteo();
};


#endif
