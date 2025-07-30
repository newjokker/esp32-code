#include <Arduino.h>
#include "WiFi.h"

// 提前声明辅助函数
String getEncryptionType(wifi_auth_mode_t encryptionType);

void setup() {
  Serial.begin(115200);
  delay(1000);  // 等待串口初始化

  // 初始化WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  Serial.println("Scanning WiFi networks...");
  
  // 开始WiFi扫描
  int numNetworks = WiFi.scanNetworks();
  
  if (numNetworks == 0) {
    Serial.println("No networks found");
  } else {
    Serial.print(numNetworks);
    Serial.println(" networks found");
    
    for (int i = 0; i < numNetworks; ++i) {
      // 打印每个网络的SSID、信号强度(RSSI)和加密类型
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm)");
      Serial.print(" [");
      Serial.print(getEncryptionType(WiFi.encryptionType(i)));
      Serial.println("]");
      delay(10);
    }
  }
  
  Serial.println("---------------------");
  
  // 等待5秒后重新扫描
  delay(5000);
}

// 辅助函数：将加密类型转换为可读字符串
String getEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case WIFI_AUTH_OPEN: return "Open";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA";
    case WIFI_AUTH_WPA2_PSK: return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2 Enterprise";
    case WIFI_AUTH_WPA3_PSK: return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2/WPA3";
    default: return "Unknown";
  }
}