
#include <WiFi.h>
#include <WebServer.h>


"""
========== 使用说明 ==========

* 运行代码，查看 esp32 提供的 wifi 和对应的 ip

* 连接 esp32 提供的 wifi 

* 访问 ip 获取界面

"""



// 配置热点名称和密码
const char* ssid = "ESP32-AP";
const char* password = "12345678";  // 至少8位字符

// 创建WebServer对象，监听80端口
WebServer server(80);

// 处理根路径请求的回调函数
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    button { padding: 10px 20px; font-size: 16px; }
  </style>
</head>
<body>
  <h1>ESP32 Hotspot Web Server</h1>
  <p>Welcome to the ESP32 Access Point!</p>
  <button onclick="alert('Hello from ESP32!')">Click Me</button>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  
  // 启动AP模式
  WiFi.softAP(ssid, password);
  
  // 打印AP的IP地址（通常是192.168.4.1）
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // 绑定根路径处理函数
  server.on("/", handleRoot);
  
  // 启动Web服务器
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // 处理客户端请求
  server.handleClient();
}