String CurAlarmClockSet = "";
void handle_AlarmTime() {
  CurAlarmClockSet = server.arg("AlarmTime");
  String date = server.arg("AlarmDate");
  if (CurAlarmClockSet != "") {
    Serial.println("time setting: " + CurAlarmClockSet);
  }
  if (date != "") {
    Serial.println("date setting: " + date);
  }
  saveIntSet();
  server.send(200, "text/plain", "OK");
}

// Установка параметров времянной зоны по запросу вида http://192.168.0.101/TimeZone?timezone=3
void handle_time_zone() {
  timezone = server.arg("timezone").toInt(); // Получаем значение timezone из запроса конвертируем в int сохраняем в глобальной переменной
  saveIntSet();
  server.send(200, "text/plain", "OK");
}

void handle_Time() {
  saveIntSet();
  server.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
}

String CurrentDate() {
  int DateIndex = timeClient.getDay();
  String date = "";
  switch (DateIndex) {
    case 0:
      date = "Sunday";
      break;
    case 1:
      date = "Monday";
      break;
    case 2:
      date = "Tuesday";
      break;
    case 3:
      date = "Wednesday";
      break;
    case 4:
      date = "Thursday";
      break;
    case 5:
      date = "Friday";
      break;
    case 6:
      date = "Saturday";
      break;
  }
  return date;
}

String CurrentTime() {
  timeClient.update();
  String time = timeClient.getFormattedTime();
  return time;
}

String shift = "";
void handle_ShiftSet() {
  shift =  server.arg("shift");
  Serial.println("shift setting: " + shift);
  saveIntSet();
  server.send(200, "text/plain", "OK");  
  
}
 
