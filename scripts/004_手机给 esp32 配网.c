#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h> // 可选：保存Wi-Fi凭证

"""
========== 使用说明 ==========

* 启动esp32

* 手机连接 ESP32 的热点 ESP32-Config（密码 12345678）。

* 浏览器访问 http://192.168.4.1，填写家庭 Wi-Fi 信息并提交。

* ESP32 自动尝试连接指定 Wi-Fi，成功后关闭自身热点。

"""


// 默认AP配置
const char* apSSID = "ESP32-Config";
const char* apPassword = "12345678"; // AP密码（至少8位）

WebServer server(80);

// 存储接收到的Wi-Fi凭证
String receivedSSID = "";
String receivedPassword = "";

// 根目录页面（配网表单）
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin-top: 20px; }
    input, button { padding: 10px; margin: 5px; }
  </style>
</head>
<body>
  <h1>ESP32 Wi-Fi配置</h1>
  <form action="/configure" method="post">
    <input type="text" name="ssid" placeholder="Wi-Fi名称" required><br>
    <input type="password" name="password" placeholder="Wi-Fi密码" required><br>
    <button type="submit">提交</button>
  </form>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

// 处理表单提交
void handleConfigure() {

  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect(); // 先断开已有连接
    delay(100);
  }
  receivedSSID = server.arg("ssid");
  receivedPassword = server.arg("password");
  
  server.send(200, "text/html", "<h1>配置已接收，ESP32正在连接Wi-Fi...</h1>");
  Serial.println("Received Wi-Fi Credentials:");
  Serial.println("SSID: " + receivedSSID);
  Serial.println("Password: " + receivedPassword);

  // 尝试连接目标Wi-Fi
  WiFi.begin(receivedSSID.c_str(), receivedPassword.c_str());
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to: " + receivedSSID);
    Serial.println("IP地址: " + WiFi.localIP().toString());
    // 关闭AP和WebServer（可选）
    WiFi.softAPdisconnect(true);
    server.stop();
  } else {
    Serial.println("\n连接失败，请重试！");
  }
}

void setup() {
  Serial.begin(115200);
  
  // 启动AP模式
  WiFi.softAP(apSSID, apPassword);
  Serial.print("AP IP地址: ");
  Serial.println(WiFi.softAPIP());

  // 配置WebServer路由
  server.on("/", handleRoot);
  server.on("/configure", HTTP_POST, handleConfigure);
  server.begin();
}

void loop() {
  server.handleClient();
}