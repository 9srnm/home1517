#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

int auto_light, light_val, light_status, vent_status, heat_status, red, green, blue, auto_heat, temp_auto, httpCode;

#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "POVED19";       // имя вашей wifi точки доступа
const char* password = ""; // пароль wifi
DynamicJsonDocument doc(1024);

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  dht.begin();

  pinMode(A0, INPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
}


void loop()
{
  String c;
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
//  Serial.println(hum);
//  Serial.println(temp);

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    HTTPClient http;  //Declare an object of class HTTPClient
//    Serial.println(String(light_val));
    http.begin("https://home.sklyar.app/second_floor?light_val=" + String(light_val) + "&hum=" + String(hum, 2) + "&temp=" + String(temp, 2), "3E 0A 7F AE 69 FE 1B A5 A6 AB CA F1 B0 8C 3A 1E F9 26 5A 5D");  //Specify request destination
    httpCode = http.GET();                                  //Send the request

    if (httpCode > 0) { //Check the returning code

      String payload = http.getString();   //Get the request response payload
      c = payload;
    }
    else {
      Serial.println(httpCode);
    }

    http.end();   //Close connection

  }
  else {
    Serial.println("No connection to Wi-Fi!");
  }

  String a = c.substring(c.indexOf('{'));
  deserializeJson(doc, a);

  Serial.println(a);
  if (httpCode > 0) {
    red = doc["red2"];
    green = doc["green2"];
    blue = doc["blue2"];
    auto_light = doc["auto_light2"];
    light_status = doc["light_status2"];
    vent_status = doc["vent"];
    heat_status = doc["heat"];
    auto_heat = doc["auto_heat"];
    temp_auto = doc["temp_auto"];
    if (auto_heat) {
      if (temp < temp_auto) {
        vent_status = 0;
        heat_status = 1;
      }
      else if (temp_auto <= temp && temp < temp_auto + 1) {
        vent_status = 0;
        heat_status = 0;
      }
      else {
        vent_status = 1;
        heat_status = 0;
      }
    }
  }

  to_slave(red, green, blue, light_status, auto_light, vent_status, heat_status);
}


void to_slave(int r, int g, int b, int l_s, int a_l, int v_s, int h_s)
{
  light_val = analogRead(A0);
  if (a_l == 1)
  {
    if (light_val < 750) {
      l_s = 0;
    }
    else {
      l_s = 1;
    }
  }

  Wire.beginTransmission(8);
  Wire.write(r);
  Wire.write(g);
  Wire.write(b);
  Wire.write(l_s);
  Wire.write(v_s);
  Wire.write(h_s);
  Wire.endTransmission();
  return;
}