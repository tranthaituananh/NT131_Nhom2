//Thư viện cần sử dụng
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define DHTPIN 2   // Chân kỹ thuật số kết nối với DHT11
#define DHTTYPE DHT11   // Đặt loại cảm biến là DHT11

DHT dht(DHTPIN, DHTTYPE);

const char *ssid = ""; // Nhập ssid wifi
const char *password = ""; // Nhập pass wifi
const char *server_url = "https://ancient-spire-87457.herokuapp.com/"; // Nodejs application endpoint

// Khai báo các biến được sử dụng để lưu thông số được đo từ cảm biến
float humi;
float tempC;
float tempF; 

StaticJsonBuffer<200> jsonBuffer;

// Thiết lập đối tượng client
WiFiClientSecure client;
HTTPClient http;
  
void setup() {
  delay(3000);
  Serial.begin(9600);

  // Khởi động cảm biến
  dht.begin();

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

  // Kiểm tra có đọc được dữ liệu từ sensor hay không   
   if (isnan(humi) || isnan(tempC) || isnan(tempF)) {
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
  Serial.println("oF");

  // Gán giá trị
  JsonObject& values = jsonBuffer.createObject();
  values["humidity"] = humi;
  values["tempC"] = tempC;
  values["tempF"] = tempF;
  
  // Tạo kết nối server và gửi dữ liệu 
  String h = "";
  h.concat(humi);
  http.begin(client, "https://ancient-spire-87457.herokuapp.com/sensor/humidity");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("humidity: " + h);  
  
  // Kiểm tra phản hồi của server với độ ẩm (httpCode = 200, 404, 500)
  if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_NOT_FOUND || httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR){
    Serial.print("Response Humidity: ");
    Serial.println(http.getString());
    Serial.print("HttpCode: ");
    Serial.println(httpCode);
  }
  else{
    Serial.printf("[HTTP] GET... failed, error: %s", http.errorToString(httpCode).c_str());
    Serial.print(" - httpcode: ");
    Serial.println(httpCode);
  }

  String tC = "";
  tC.concat(tempC);
  String tF = "";
  tF.concat(tempF);
  http.begin(client, "https://ancient-spire-87457.herokuapp.com/sensor/temperature");
  http.addHeader("Content-Type", "application/json");
  int httpCode1 = http.POST("celcius: " + tC + "fahrenheit: " + tF);
  
  // Kiểm tra phản hồi của server với độ ẩm (httpCode = 200, 404, 500)
  if(httpCode1 == HTTP_CODE_OK || httpCode1 == HTTP_CODE_NOT_FOUND || httpCode1 == HTTP_CODE_INTERNAL_SERVER_ERROR){
    Serial.print("Response Temperature: ");
    Serial.println(http.getString());
    Serial.print("HttpCode: ");
    Serial.println(httpCode);
  }
  else{
    Serial.printf("[HTTP] GET... failed, error: %s", http.errorToString(httpCode).c_str());
    Serial.print(" - HttpCode: ");
    Serial.println(httpCode);
  }

  http.end(); // Ngắt kết nối
  delay(3000); //Gửi dữ liệu 5p/lần 
}
