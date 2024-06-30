#include <WiFi.h>
#include <esp_now.h>

// 定义服务端的MAC地址 cc:7b:5c:36:19:80 // 10:06:1c:b5:ef:4c
//uint8_t serverMac[] = {0xCC, 0x7B, 0x5C, 0x36, 0x19, 0x80};
uint8_t serverMac[] = {0x10, 0x06, 0x1C, 0xB5, 0xEF, 0x4C};
esp_now_peer_info_t peerInfo = {};


// ESP-NOW消息发送状态的回调函数
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus) {
  // Serial.print("Data send status: ");
  if (sendStatus == ESP_NOW_SEND_SUCCESS) {
    // Serial.println("Success");
  } else {
    // Serial.println("Data send status: Failure");
  }
}

bool checkESPNOW() {
  return esp_now_is_peer_exist(peerInfo.peer_addr);
}

void initESPNOW(){
  // 初始化ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // 设置ESP-NOW的角色和回调函数
  esp_now_register_send_cb(OnDataSent);

  // 添加服务端为对等设备

  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void setup() {
  Serial.begin(9600);
  // 初始化WiFi为Station模式
  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Sender setup");
  memcpy(peerInfo.peer_addr, serverMac, 6);

  initESPNOW();

  // 初始化ADC
  analogReadResolution(12); // 设置ADC分辨率为12位

  // 初始化GPIO 输入, 使用内部上拉电阻
  pinMode(13, INPUT_PULLUP); 
  pinMode(32, INPUT_PULLUP); 
  pinMode(33, INPUT_PULLUP); 
  pinMode(25, INPUT_PULLUP); 
  pinMode(26, INPUT_PULLUP); 
  pinMode(27, INPUT_PULLUP); 
  pinMode(14, INPUT_PULLUP); 
  pinMode(12, INPUT_PULLUP); 
}

void loop() {

  if (!checkESPNOW()) {
    Serial.println("Reinitializing ESP-NOW");
    esp_now_deinit();
    delay(1000); // 等待1秒
    initESPNOW();
  }

  int xValue = analogRead(34); // 读取X轴的模拟值
  int yValue = analogRead(35); // 读取Y轴的模拟值

  // 读取摇杆按钮状态
  int buttonState = digitalRead(13); 

  // 读取自定义按钮状态
  int buttonState32 = digitalRead(32);
  int buttonState33 = digitalRead(33); 
  int buttonState25 = digitalRead(25); 
  int buttonState26 = digitalRead(26); 
  int buttonState27 = digitalRead(27); 
  int buttonState14 = digitalRead(14); 
  int buttonState12 = digitalRead(12); 

  // Serial.println(xValue);

  char command;

  // 确定命令基于X和Y轴的读数
  if (yValue > 2048 + 500) {
    command = 'F'; // 前进
  } else if (yValue < 2048 - 500) {
    command = 'B'; // 后退
  } else if (xValue > 2048 + 500) {
    command = 'R'; // 右转
  } else if (xValue < 2048 - 500) {
    command = 'L'; // 左转
  } else {
    command = 'T'; // 停止
  }

  if (buttonState == LOW) { 
    command = 'K'; 
  } else if (buttonState32 == LOW) { 
    command = 'Y'; 
    Serial.println("gpio32 Y");
  } else if (buttonState33 == LOW) { 
    command = 'G'; 
    Serial.println("gpio33 G");
  } else if (buttonState25 == LOW) { 
    command = 'r'; 
    Serial.println("gpio25 r");
  } else if (buttonState26 == LOW) { 
    command = 'S'; 
    Serial.println("gpio26 S");
  } else if (buttonState27 == LOW) { 
    command = 'A'; 
    Serial.println("gpio27 A");
  } else if (buttonState14 == LOW) { 
    command = 'W'; 
    Serial.println("gpio14 W");
  } else if (buttonState12 == LOW) { 
    command = 'D'; 
    Serial.println("gpio12 D");
  }

  // 发送命令
  esp_now_send(serverMac, (uint8_t *)&command, sizeof(command));
  delay(100); 
}
