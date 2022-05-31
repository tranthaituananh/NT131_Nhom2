//Thư viện cần sử dụng
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#define DHTPIN 2   // Chân kỹ thuật số kết nối với DHT11
#define DHTTYPE DHT11   // Đặt loại cảm biến là DHT11

DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "Thanh Thai"; // Nhập ssid wifi
const char *password = "Nevergiveup2705"; // Nhập pass wifi
const char *server_url = "https://ancient-spire-87457.herokuapp.com/"; // Nodejs application endpoint

// Khai báo các biến được sử dụng để lưu thông số được đo từ cảm biến
float humi;
float tempC;
float tempF; 
int cambien = 5;
int giatri;

// Thiết lập đối tượng client
WiFiClient client;
HTTPClient http;
  
void setup() {
  delay(3000);
  Serial.begin(9600);

  // Khởi động cảm biến
  dht.begin();
  pinMode(cambien, INPUT);

  //Kết nối wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected"); // Kết nối wifi thành công
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // Lấy địa chỉ IP của thiết bị
  delay(1000);
}

void loop() {
  // Lấy thông số từ cảm biến
  humi = dht.readHumidity();
  tempC = dht.readTemperature();
  tempF = dht.readTemperature(true);
  giatri = digitalRead(cambien);

  // Kiểm tra có đọc được dữ liệu từ sensor hay không   
   if (isnan(humi) || isnan(tempC) || isnan(tempF) || isnan(giatri)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }         
  
  // Hiển thị kết quả thu được lên màn hình
  Serial.print("Humidity = ");
  Serial.print(humi);
  Serial.print("%  ");
  Serial.print("Temperature = ");
  Serial.print(tempC); 
  Serial.print("oC ~ ");
  Serial.print(tempF); 
  Serial.print("oF  ");
  Serial.print("Photoresistance Sensor = ");
  Serial.println(giatri);

  // Tạo kết nối server và gửi dữ liệu 
  String h = "";
  h.concat(humi);
  http.begin(client, "http://192.168.1.238:3000/sensor/humidity");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("{\"humidity\":" + h + "}");  
  
  // Kiểm tra phản hồi của server với độ ẩm (httpCode = 200, 404, 500)
  if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_NOT_FOUND || httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR){
    Serial.print("Response Humidity: ");
    Serial.println(http.getString());
    Serial.print("HttpCode: ");
    Serial.println(httpCode);
  }
  else{
    Serial.printf("Error humidity: %s", http.errorToString(httpCode).c_str());
    Serial.print(" - httpcode: ");
    Serial.println(httpCode);
  }

  String tC = "";
  tC.concat(tempC);
  String tF = "";
  tF.concat(tempF);
  http.begin(client, "http://192.168.1.238:3000/sensor/temperature");
  http.addHeader("Content-Type", "application/json");
  int httpCode1 = http.POST("{\"celcius\":" + tC + ",\"fahrenheit\":" + tF + "}");
  
  // Kiểm tra phản hồi của server với độ ẩm (httpCode = 200, 404, 500)
  if(httpCode1 == HTTP_CODE_OK || httpCode1 == HTTP_CODE_NOT_FOUND || httpCode1 == HTTP_CODE_INTERNAL_SERVER_ERROR){
    Serial.print("Response Temperature: ");
    Serial.println(http.getString());
    Serial.print("HttpCode: ");
    Serial.println(httpCode);
  }
  else{
    Serial.printf("Error temperature: %s", http.errorToString(httpCode).c_str());
    Serial.print(" - HttpCode: ");
    Serial.println(httpCode);
  }

  String l = "";
  l.concat(giatri);
  http.begin(client, "http://192.168.1.238:3000/sensor/light");
  http.addHeader("Content-Type", "application/json");
  int httpCode2 = http.POST("{\"light\":" + l + "}");  
  
  // Kiểm tra phản hồi của server với độ ẩm (httpCode = 200, 404, 500)
  if(httpCode2 == HTTP_CODE_OK || httpCode2 == HTTP_CODE_NOT_FOUND || httpCode2 == HTTP_CODE_INTERNAL_SERVER_ERROR){
    Serial.print("Response Light: ");
    Serial.println(http.getString());
    Serial.print("HttpCode: ");
    Serial.println(httpCode);
  }
  else{
    Serial.printf("Error light: %s", http.errorToString(httpCode).c_str());
    Serial.print(" - httpcode: ");
    Serial.println(httpCode);
  }

  http.end(); // Ngắt kết nối
  Serial.println("--------------------");
  delay(5000); //Gửi dữ liệu 5s/lần 
}
