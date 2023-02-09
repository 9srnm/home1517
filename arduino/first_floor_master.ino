#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

const char *ssid = "POVED19";       // имя вашей wifi точки доступа
const char *password = ""; // пароль wifi
DynamicJsonDocument doc(1024);

int auto_light, light_val, light_status, red, green, blue,
door, httpCode;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
}

void loop() {
  String c;
//  client.print(String("GET /first_floor\r\nHost: " + String(host) + "\r\nConnection: close\r\n\r\n"));

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    HTTPClient http;  //Declare an object of class HTTPClient
//    Serial.println(String(light_val));
    http.begin("https://home.sklyar.app/first_floor", "3E 0A 7F AE 69 FE 1B A5 A6 AB CA F1 B0 8C 3A 1E F9 26 5A 5D");  //Specify request destination
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
    red = doc["red1"];
    green = doc["green1"];
    blue = doc["blue1"];
    auto_light = doc["auto_light1"];
    light_status = doc["light_status1"];
    light_val = doc["light_val"];
    door = doc["door"];

    lights(red, green, blue, light_status, auto_light, light_val, door);
  }
}


void lights(int r, int g, int b, int l_s, int a_l, int l_v, int d_s)
{
  if (a_l == 1)
  {
    if (l_v < 750) {
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
  Wire.write(d_s);
  Wire.endTransmission();
}