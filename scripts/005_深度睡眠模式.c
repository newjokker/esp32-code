#include <esp_sleep.h>
#include <driver/rtc_io.h>

#define BUTTON_PIN 0  // 使用BOOT按钮作为唤醒源
#define LED_PIN 48    // 使用板载LED或连接一个LED

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // 打印唤醒原因
  print_wakeup_reason();
  
  Serial.println("\n选择睡眠模式:");
  Serial.println("1 - 活跃模式 (不睡眠)");
  Serial.println("2 - 轻度睡眠模式");
  Serial.println("3 - 深度睡眠模式");
  Serial.println("按BOOT按钮选择对应模式");
  
  // 等待用户选择
  int mode = waitForUserSelection();
  
  switch(mode) {
    case 1:
      activeModeDemo();
      break;
    case 2:
      lightSleepDemo();
      break;
    case 3:
      deepSleepDemo();
      break;
    default:
      Serial.println("无效选择，使用活跃模式");
      activeModeDemo();
  }
}

void loop() {
  // 在活跃模式外，不会执行到这里
}

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  
  switch(wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("通过外部信号(RTC_IO)唤醒");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("通过外部信号(RTC_CNTL)唤醒");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("通过定时器唤醒");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("通过触摸唤醒");
      break;
    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println("通过ULP程序唤醒");
      break;
    case ESP_SLEEP_WAKEUP_GPIO:
      Serial.println("通过GPIO唤醒(轻度睡眠)");
      break;
    case ESP_SLEEP_WAKEUP_UART:
      Serial.println("通过UART唤醒(轻度睡眠)");
      break;
    default:
      Serial.printf("不是被睡眠唤醒: %d\n", wakeup_reason);
      break;
  }
}

int waitForUserSelection() {
  unsigned long startTime = millis();
  int selection = 0;
  
  while(millis() - startTime < 10000) { // 10秒超时
    if(digitalRead(BUTTON_PIN) == LOW) {
      selection++;
      if(selection > 3) selection = 1;
      
      Serial.print("已选择模式: ");
      Serial.println(selection);
      
      // 视觉反馈
      for(int i=0; i<selection; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
      }
      
      // 等待按钮释放
      while(digitalRead(BUTTON_PIN) == LOW) {
        delay(10);
      }
      
      delay(500); // 防抖
    }
    
    if(selection != 0) {
      delay(2000); // 给用户2秒确认时间
      return selection;
    }
    
    delay(100);
  }
  
  Serial.println("超时，默认使用深度睡眠模式");
  return 3;
}

void activeModeDemo() {
  Serial.println("\n活跃模式 - 最高功耗");
  Serial.println("CPU全速运行，不进入任何睡眠状态");
  Serial.println("LED将持续闪烁");
  
  while(true) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(900);
    
    // 检查是否要切换到其他模式
    if(digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("检测到按钮按下，准备切换模式...");
      delay(1000);
      ESP.restart();
    }
  }
}

void lightSleepDemo() {
  Serial.println("\n轻度睡眠模式 - 中等功耗");
  Serial.println("CPU暂停，外设和内存保持供电");
  Serial.println("可通过多种方式唤醒");
  
  // 配置GPIO唤醒源
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);
  
  Serial.println("进入轻度睡眠模式...");
  Serial.println("按BOOT按钮唤醒");
  
  while(true) {
    // 闪烁LED表示即将进入睡眠
    for(int i=0; i<3; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
    
    // 进入轻度睡眠
    esp_light_sleep_start();
    
    // 唤醒后执行
    Serial.println("从轻度睡眠唤醒");
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    
    // 短暂延迟后再次进入睡眠
    delay(1000);
  }
}

void deepSleepDemo() {
  Serial.println("\n深度睡眠模式 - 最低功耗");
  Serial.println("CPU、大多数外设和内存断电");
  Serial.println("仅RTC控制器和ULP协处理器保持活动");
  
  // 配置唤醒源
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);
  
  // 也可以设置定时唤醒（这里设置为20秒，用于演示）
  esp_sleep_enable_timer_wakeup(20 * 1000000);
  
  Serial.println("准备进入深度睡眠...");
  Serial.println("可以通过以下方式唤醒:");
  Serial.println("1. 按下BOOT按钮");
  Serial.println("2. 20秒后定时唤醒");
  
  // 闪烁LED表示即将进入深度睡眠
  for(int i=0; i<5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  
  Serial.println("进入深度睡眠...");
  Serial.flush(); // 确保所有数据已发送
  
  // 断开不需要保持的GPIO
  gpio_deep_sleep_hold_dis();
  
  // 进入深度睡眠
  esp_deep_sleep_start();
}