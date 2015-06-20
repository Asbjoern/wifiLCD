#include <TFTv2.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include "settings.h"
#include "debughandler.h"

#define RUNLED 4 //GPIO 4
#define BLINKTIME 250 //ms
#define WDTTIME WDTO_8S //8sec
#define CONNECTTIMEOUT 20 //x500ms
#define COMTIMEOUT 2000 //ms

extern struct SettingsStruct settings;
void bmpDraw(WiFiClient f);

MDNSResponder mdns;

ESP8266WebServer server(80);
WiFiClient client;
unsigned long blinkTime,GETtimer;
bool softAP = false;
 
void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');
    
  lcd_init();
  initConfig();
  debug_init();
  
  if(settings.runLED)
  {
    pinMode(RUNLED,OUTPUT);
    digitalWrite(RUNLED,LOW);
  }
  connectWifi();

 if(settings.useWDT) 
   ESP.wdtEnable(WDTTIME);
 else
   ESP.wdtDisable();
  
  server_init();
  debugPrintln("HTTP server started");
  client.setTimeout(COMTIMEOUT);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED && !softAP)
  {
    connectWifi();
  }
  if(settings.useWDT)  
    ESP.wdtFeed();
    
  if(settings.runLED && millis()>blinkTime)
  {
    blinkTime += BLINKTIME;
    if(digitalRead(RUNLED))
      digitalWrite(RUNLED,LOW);
    else
      digitalWrite(RUNLED,HIGH);
  }
  
  if(settings.deepSleep)
    ESP.deepSleep(settings.deepSleepTime, WAKE_RF_DEFAULT) ; //WAKE_RF_DEFAULT, WAKE_RFCAL, WAKE_NO_RFCAL, WAKE_RF_DISABLED 

  server.handleClient();
  
  if(settings.autoGET && millis()>GETtimer)
  {
    GETtimer += settings.getInterval*1000;
    Serial.print("connecting to ");
    Serial.println(settings.host);
    client.stop();
    delay(100);
    if (!client.connect(settings.host, 80))
      debugPrintln("Connection failed",RED);
    else
    {
       Serial.print("Requesting URL: ");
       Serial.println(settings.url);
       client.print(String("GET ") + settings.url + " HTTP/1.1\r\n" +
               "Host: " + settings.host + "\r\n" + 
               "Connection: close\r\n\r\n");
       delay(10); 
       String str = client.readStringUntil('\r');
       if(str.equals("HTTP/1.1 200 OK"))
       {
         Serial.println("Got 200 OK");
         bool headerdone = false;
         while(!headerdone)
         {
          Serial.println(client.readStringUntil('\r'));
          if(client.read() == '\n' && client.read() == '\r' && client.read() == '\n')
                headerdone=true; 
         }
         Serial.println("Header read");
        
         if(bmpReadHeader(client))
         {
           Serial.println("BMP header accepted. Drawing file");
           bmpDraw(client);
           client.stop();
         }
         else
           Serial.print("BMP header rejected!!");
       }
       else
       {
         debugPrintln("GET rejected",RED);
         Serial.println(str);
       }
    }
  }
  
  delay(100);
}

void connectWifi()
{
  WiFi.disconnect();
  delay(200);
  if(settings.APmode)
  {
    createAP();
    return;
  }
  debugPrintln("Scanning Networks...");
  bool APfound = false;
  int numSsid = WiFi.scanNetworks();
  
  Serial.println("Found:");
  for(int thisNet = 0; thisNet < numSsid; thisNet++) 
  {
    Serial.println(WiFi.SSID(thisNet));
    if(strcmp(WiFi.SSID(thisNet),settings.ssid) == 0)
      APfound = true;
  }
      
  if(!APfound)
  {
    debugPrintln(settings.ssid,RED);
    debugPrintln("Not found!",RED);
    createAP();
    return;
  }
  debugPrintln("Connecting to:");
  debugPrintln(settings.ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(settings.ssid, settings.password);
  
  uint8_t cnt = 0;
  while (WiFi.status() != WL_CONNECTED && cnt < CONNECTTIMEOUT) {
    delay(500);
    debugPrint(".");
    cnt++;
  }
  if(cnt >= CONNECTTIMEOUT)
  {
    debugPrintln("Error connecting",RED);
    return;
  }
  
  debugPrintln("");
  debugPrintln("WiFi connected");  
  debugPrintln("IP address: ");
  char ip[17];
  sprintf(ip,"%d.%d.%d.%d",WiFi.localIP()[0],WiFi.localIP()[1],WiFi.localIP()[2],WiFi.localIP()[3]);
  debugPrintln(ip);
  if (mdns.begin(settings.name, WiFi.localIP())) 
  {
    debugPrintln("mDNS started");
    char name[16];
    sprintf(name,"%s.local",settings.name);
    debugPrintln(name);
  }
}

void createAP()
{
  WiFi.disconnect();
  delay(200);
  softAP=true;
  debugPrintln("Creating softAP:");
  debugPrintln(settings.ssid);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(settings.ssid, settings.password);
 
  debugPrintln("IP address: ");
  char ip[17];
  sprintf(ip,"%d.%d.%d.%d",WiFi.softAPIP()[0],WiFi.softAPIP()[1],WiFi.softAPIP()[2],WiFi.softAPIP()[3]);
  debugPrintln(ip);
  
}
