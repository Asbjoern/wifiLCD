void server_init() {
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.on("/settings", HTTP_POST, settingsHandler);
  if (settings.allowPUT)
  {
    server.on("/img", HTTP_PUT, uploadHandler);
    server.onFileUpload(handleFileUpload);
  }
  server.begin();
  server.client().setTimeout(COMTIMEOUT);
}

void handleRoot() {
  byte mac[6];
  WiFi.macAddress(mac);
  String message = F("<html><body>" \
                   "<h1>WiFi LCD" \
                   " v");
  message +=   FWVERSION;
  message +=   F("</h1><table style='width:320px'>" \
               "<tr><td>MAC:</td><td>");
  message +=   WiFi.macAddress();
  message +=    F("</td></tr>"\
                "<tr><td>Local IP:</td><td>");
  for (int i = 0; i < 4; i++) {
    message += String(WiFi.localIP()[i]);
    if (i < 3) message += ".";
  }
  message +=      F("</td></tr>"\
                  "<tr><td>Connected:</td><td>");
  message +=      WiFi.SSID();
  message +=      F("</td></tr>"\
                  "<tr><td>Chip ID:</td><td>");
  message +=      String(ESP.getChipId(), HEX);
  message +=      F("</td></tr>"\
                  "<tr><td>CPU clock:</td><td>");
  message +=      String(ESP.getCpuFreqMHz(), DEC);
  message +=      F("MHz</td></tr>"\
                  "<tr><td>Flash Chip Size:</td><td>");
  message +=      String(ESP.getFlashChipRealSize() / 1024);
  message +=      F("KB</td></tr>"\
                  "<tr><td>Flash Chip Speed:</td><td>");
  message +=      String(ESP.getFlashChipSpeed() / 1000);
  message +=      F("KHz</td></tr>"\
                  "<tr><td>Free Heap:</td><td>");
  message +=      String(ESP.getFreeHeap());
  message +=      F("B</td></tr>"\
                  "<tr><td>Sketch size:</td><td>");
  message +=      String(ESP.getSketchSize() / 1024);
  message +=      F("KB</td></tr>"\
                  "<tr><td>Free Sketch space:</td><td>");
  message +=      String(ESP.getFreeSketchSpace() / 1024);
  message +=      F("KB</td></tr>"\
                  "<tr><td>Uptime:</td><td>");
  message +=      String(millis() / 1000 / 60);
  message +=      " minutes</td></tr>";
  message +=   "</table>";
  message +=   "<br>";
  message +=   "<script> function transfer(val,id) {document.getElementById(id).value = val;} </script>";
  message +=   "<form action='/settings' method='post'>";
  message +=     "<fieldset style='width:320px'><legend>Network</legend>";
  message +=       "<table style='width:100%'>";
  message +=        "<tr><td>AP mode: </td><td><input type='checkbox' name='APmode'";
  if (settings.APmode) message += " checked ";
  message +=        "></td></tr>";
  message +=         "<tr><td>SSID: </td><td><input type='text' maxlength='31' id='ssid' name='ssid' value='";
  message +=         settings.ssid;
  message +=         "' style='width:50%'><select name='aplist' style='width:46%' onchange=\"transfer(this.value,'ssid');\">";
  if (numSsid == -1) {
    message +=  "<option value='0'>No Network</option>";
  }
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    message +=  "<option value='";
    message +=  WiFi.SSID(thisNet);
    message +=  "'";
    if (WiFi.SSID(thisNet).compareTo(WiFi.SSID()) == 0)
      message +=  " selected";
    message +=  ">";
    message +=  WiFi.SSID(thisNet);
    message += " (";
    message += WiFi.RSSI(thisNet);
    message += ")";
    if(WiFi.encryptionType(thisNet) == ENC_TYPE_NONE)
      message += "*";
    message +=  "</option>";
  }
  message +=         "</select></td></tr>";
  message +=         "<tr><td>Password: </td><td><input type='text' maxlength='31' name='password' value='";
  message +=         settings.password;
  message +=         "'> </td></tr>";
  message +=         "<tr><td>mDNS name: </td><td><input type='text' maxlength='31' name='name' value='";
  message +=         settings.name;
  message +=         "' style='width:130px'>.local </td></tr>";
  message +=      "</table>";
  message +=    "</fieldset>";
  message +=    "<fieldset style='width:320px'><legend>System</legend>";
  message +=      "<table style='width:100%'>";
  message +=        "<tr><td>Run LED: </td><td><input type='checkbox' name='runLED'";
  if (settings.runLED) message += " checked ";
  message +=        "></td></tr>";
  message +=        "<tr><td>Serial DEBUG: </td><td><input type='checkbox' name='SerDebug'";
  if (settings.SerDebug) message += " checked ";
  message +=        "></td></tr>";
  message +=        "<tr><td>LCD Debug: </td><td><input type='checkbox' name='LcdDebug'";
  if (settings.LcdDebug) message += " checked ";
  message +=        "></td></tr>";
  message +=    "</table>";
  message +=  "</fieldset>";
  message +=  "<fieldset style='width:320px'><legend>Image</legend>";
  message +=  "<table style='width:100%'>";
  message +=    "<tr><td>Backlight: </td><td>";
  message +=      "<input type=range min=0 max=1024 id=blslider step=1 style='width:120px' oninput=\"transfer(value,'backlight');\" value='";
  message +=         settings.backlight;
  message +=      "'><input type='number' id='backlight' name='backlight' style='width:45px' onchange=\"transfer(this.value,'blslider');\" value='";
  message +=         settings.backlight;
  message +=      "'></td></tr>";
  message +=    "<tr><td>Allow PUT: </td><td><input type='checkbox' name='allowPUT' ";
  if (settings.allowPUT) message += " checked ";
  message +=        "></td></tr>";
  message +=    "<tr><td>Auto GET: </td><td><input type='checkbox' name='autoGET'";
  if (settings.autoGET) message += " checked ";
  message +=        "></td></tr>";
  message +=    "<tr><td>Auto GET interval: </td><td><input type='number' name='getInterval' style='width:130px' value='";
  message +=         settings.getInterval;
  message +=         "'>sec </td></tr>";
  message +=    "<tr><td>Auto GET url: </td><td><input type='text' maxlength='31' name='getUrl' value='";
  message +=         settings.url;
  message +=         "' ></td></tr>";
  message +=  "</table>";
  message +=  "</fieldset>";
  message += "<input type='submit' value='Submit'>";
  message += "</form>";
  message += "</body></html>";
  server.send(200, "text/html", message);
  //yield();
}

void handleNotFound()
{

  server.send(404, "text/plain", "File Not Found\n\n");
}

void settingsHandler() {
  String message = "Settings saved\n\nPlease wait for reboot!\n\n";
  settings.APmode = false;
  settings.runLED = false;
  settings.SerDebug = false;
  settings.LcdDebug = false;
  settings.allowPUT = false;
  settings.autoGET = false;
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "APmode") settings.APmode = true;
    else if (server.argName(i) == "ssid") urldecode(settings.ssid, server.arg(i).c_str());
    else if (server.argName(i) == "password") urldecode(settings.password, server.arg(i).c_str());
    else if (server.argName(i) == "name") urldecode(settings.name, server.arg(i).c_str());
    else if (server.argName(i) == "runLED") settings.runLED = true;
    else if (server.argName(i) == "SerDebug") settings.SerDebug = true;
    else if (server.argName(i) == "LcdDebug") settings.LcdDebug = true;
    else if (server.argName(i) == "backlight") settings.backlight = atoi(server.arg(i).c_str());
    else if (server.argName(i) == "allowPUT") settings.allowPUT = true;
    else if (server.argName(i) == "autoGET") settings.autoGET = true;
    else if (server.argName(i) == "getInterval") settings.getInterval = atoi(server.arg(i).c_str());
    else if (server.argName(i) == "getUrl") urldecode(settings.url, server.arg(i).c_str());
    // message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  saveConfig();
  server.send(200, "text/plain", message);
  delay(2000);
  ESP.restart();
}

void uploadHandler()
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "");
}

void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    Serial.print("Upload: START, filename: "); Serial.println(upload.filename);
    bmpPUTinit();
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    bmpPUT(upload);
  }
  else if (upload.status == UPLOAD_FILE_END) {
    Serial.print("Upload: END, Size: "); Serial.println(upload.totalSize);
  }
}

void urldecode(char *dst, const char *src)
{
  char a, b;
  while (*src) {
    if ((*src == '%') &&
        ((a = src[1]) && (b = src[2])) &&
        (isxdigit(a) && isxdigit(b))) {
      if (a >= 'a')
        a -= 'a' - 'A';
      if (a >= 'A')
        a -= ('A' - 10);
      else
        a -= '0';
      if (b >= 'a')
        b -= 'a' - 'A';
      if (b >= 'A')
        b -= ('A' - 10);
      else
        b -= '0';
      *dst++ = 16 * a + b;
      src += 3;
    }
    else {
      *dst++ = *src++;
    }
  }
  *dst++ = '\0';
}
