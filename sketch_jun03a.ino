#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include "LiquidCrystal_I2C.h"

#include<Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

 int RX= 2, TX= 3;
static const uint32_t GPSBaud = 9600;
const char* ssid     = "ASUS_VIJAY_2G";//Network SSID
const char* password = "8951107035";//Network Password
unsigned long ch_no = 1406999;// Thingspeak Channel ID
const char * write_api = "0DZVL35RY2OHP33R";// Thingspeak write API

TinyGPSPlus gps;
WiFiClient  client;
WiFiServer server(80);

SoftwareSerial soft(RX, TX);
String latitude_data;
String longitude_data;

void setup()
{
  Wire.begin(D6,D5);
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("   IOT BASED       ");
  lcd.setCursor(0, 1);
  lcd.print("VEHICLE TRACKING   ");
  delay(2000);
  lcd.clear(); 
  Serial.begin(9600);
  soft.begin(GPSBaud);
  WiFi.begin(ssid, password);
  server.begin();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    lcd.setCursor(0, 0);
    lcd.print("WiFi connecting...          ");
  }
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected          ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  ThingSpeak.begin(client);
}
void loop()
{
  while (soft.available() > 0)
    if (gps.encode(soft.read()))
    {
      
      displaydata();
      displaywebpage();
    }

    
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("GPS Connection Error!!"));
    while (true);
  }
}
void displaydata()
{
  if (gps.location.isValid())
  {
    double latitude = (gps.location.lat());
    double longitude = (gps.location.lng());
    Serial.print("Latitude: " );
    Serial.print(latitude);
    Serial.print("Longitude: " );
    Serial.print(longitude);
    latitude_data= (String(latitude, 6));
    longitude_data= (String(longitude, 6));
    ThingSpeak.setField(1, latitude_data);
    ThingSpeak.setField(2, longitude_data);
    ThingSpeak.writeFields(ch_no, write_api);
    delay(20000);
  }
  else
  {
    Serial.println("Location: Not Available");
  }
}
void displaywebpage()
{
    WiFiClient client = server.available();
    if (!client)
    {
      return;
    }
    String page = "<html><center><p><h1>Real Time Vehicle Tracking using IoT</h1><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
      page += latitude_data;
    page += "+";
    page += longitude_data;
    page += "><button>Click here For Live Location</button></a> </p></center>";
    
    page += "<table> <th> <td> </style> </head> <body> <h2  style=";
    page += "font-size:300%;";
    page += " ALIGN=CENTER> GPS DATA</h2>";
    page += "<p ALIGN=CENTER pagetyle=""font-size:150%;""";
    page += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
    page += "width:50%";
    page += "> <tr> <th>Latitude</th>";
    page += "<td ALIGN=CENTER >";
    page += latitude_data;
    page += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
    page += longitude_data;

    page += "</table></body> </html>";
 

    ;
    
    client.print(page);
    delay(100);
}
