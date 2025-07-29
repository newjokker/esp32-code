#include <WiFi.h>
#include <esp_wifi.h>

"""
========== 使用说明 ==========

* 运行代码

* 连接 esp32 提供的 wifi 

* esp32 打印连接设备的信息 

"""


const char *ssid = "ESP32S3_AP";      // 设置AP的SSID
const char *password = "12345678";    // 设置AP的密码(至少8个字符)

void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      Serial.printf("Device connected, MAC: %02x:%02x:%02x:%02x:%02x:%02x, AID: %d\n",
                   info.wifi_ap_staconnected.mac[0], 
                   info.wifi_ap_staconnected.mac[1], 
                   info.wifi_ap_staconnected.mac[2],
                   info.wifi_ap_staconnected.mac[3], 
                   info.wifi_ap_staconnected.mac[4], 
                   info.wifi_ap_staconnected.mac[5],
                   info.wifi_ap_staconnected.aid);
      break;
      
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      Serial.printf("Device disconnected, MAC: %02x:%02x:%02x:%02x:%02x:%02x, AID: %d\n",
                   info.wifi_ap_stadisconnected.mac[0], 
                   info.wifi_ap_stadisconnected.mac[1], 
                   info.wifi_ap_stadisconnected.mac[2],
                   info.wifi_ap_stadisconnected.mac[3], 
                   info.wifi_ap_stadisconnected.mac[4], 
                   info.wifi_ap_stadisconnected.mac[5],
                   info.wifi_ap_stadisconnected.aid);
      break;
      
    default:
      break;
  }
}

void listConnectedStations() {
  wifi_sta_list_t stationList;
  if (esp_wifi_ap_get_sta_list(&stationList) == ESP_OK) {
    Serial.printf("Number of connected stations: %d\n", stationList.num);
    
    for (int i = 0; i < stationList.num; i++) {
      wifi_sta_info_t station = stationList.sta[i];
      Serial.printf("Station %d - MAC: %02x:%02x:%02x:%02x:%02x:%02x, RSSI: %d\n", 
                   i+1,
                   station.mac[0], station.mac[1], station.mac[2],
                   station.mac[3], station.mac[4], station.mac[5],
                   station.rssi);
    }
  } else {
    Serial.println("Failed to get station list");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.onEvent(WiFiEvent);
  
  // 创建WiFi接入点
  WiFi.softAP(ssid, password);
  
  Serial.println("WiFi Access Point started");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  static unsigned long lastPrintTime = 0;
  
  // 每5秒打印一次当前连接的设备列表
  if (millis() - lastPrintTime > 5000) {
    lastPrintTime = millis();
    listConnectedStations();
    Serial.println("---------------------");
  }
  
  delay(100);
}