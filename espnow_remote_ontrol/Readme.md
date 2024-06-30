# ESP-NOW 控制器设计与实现

ESP-NOW 是一种由 Espressif 开发的用于 ESP8266 和 ESP32 设备的无线通信协议。它允许设备之间进行快速、低功耗的数据传输，非常适合在物联网（IoT）应用中使用。本文将介绍如何使用 ESP-NOW 创建一个无线控制器，并详细讲解其工作原理和实现过程。

## 硬件准备

在开始编程之前，我们需要以下硬件：
- 一块 ESP32 开发板
- 一个摇杆模块（带有两个模拟输出和一个按钮）
- 几个按钮，用于发送不同的命令
- 连接线和面包板

## 编程实现

以下是完整的代码实现，它实现了一个 ESP32 基于 ESP-NOW 的无线控制器：

```cpp
#include <WiFi.h>
#include <esp_now.h>

// 定义服务端的MAC地址
uint8_t serverMac[] = {0x10, 0x06, 0x1C, 0xB5, 0xEF, 0x4C};
esp_now_peer_info_t peerInfo = {};

// ESP-NOW消息发送状态的回调函数
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus) {
  if (sendStatus == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Data send status: Success");
  } else {
    Serial.println("Data send status: Failure");
  }
}

// 检查ESP-NOW对等设备是否存在
bool checkESPNOW() {
  return esp_now_is_peer_exist(peerInfo.peer_addr);
}

// 初始化ESP-NOW
void initESPNOW() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Sender setup");
  memcpy(peerInfo.peer_addr, serverMac, 6);

  initESPNOW();

  analogReadResolution(12);

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
    delay(1000);
    initESPNOW();
  }

  int xValue = analogRead(34);
  int yValue = analogRead(35);
  int buttonState = digitalRead(13); 
  int buttonState32 = digitalRead(32);
  int buttonState33 = digitalRead(33); 
  int buttonState25 = digitalRead(25); 
  int buttonState26 = digitalRead(26); 
  int buttonState27 = digitalRead(27); 
  int buttonState14 = digitalRead(14); 
  int buttonState12 = digitalRead(12); 

  char command;

  if (yValue > 2048 + 500) {
    command = 'F';
  } else if (yValue < 2048 - 500) {
    command = 'B';
  } else if (xValue > 2048 + 500) {
    command = 'R';
  } else if (xValue < 2048 - 500) {
    command = 'L';
  } else {
    command = 'T';
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

  esp_now_send(serverMac, (uint8_t *)&command, sizeof(command));
  delay(100); 
}
```

### 代码讲解

#### 包含必要的库

首先，我们包含了两个必要的库：

```cpp
#include <WiFi.h>
#include <esp_now.h>
```

`WiFi.h` 用于设置 ESP32 的 Wi-Fi 模式，而 `esp_now.h` 则提供了 ESP-NOW 通信的支持。

#### 定义服务器 MAC 地址和 ESP-NOW 相关变量

我们需要定义服务端的 MAC 地址，并初始化一个 `esp_now_peer_info_t` 结构体变量 `peerInfo`：

```cpp
uint8_t serverMac[] = {0x10, 0x06, 0x1C, 0xB5, 0xEF, 0x4C};
esp_now_peer_info_t peerInfo = {};
```

#### 回调函数

定义一个回调函数 `OnDataSent`，用于处理数据发送状态：

```cpp
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus) {
  if (sendStatus == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Data send status: Success");
  } else {
    Serial.println("Data send status: Failure");
  }
}
```

#### 初始化 ESP-NOW

在 `initESPNOW` 函数中，我们初始化 ESP-NOW，并添加服务端作为对等设备：

```cpp
void initESPNOW() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}
```

#### 设置函数

在 `setup` 函数中，我们初始化串口通信、Wi-Fi 模式，并调用 `initESPNOW` 初始化 ESP-NOW：

```cpp
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Sender setup");
  memcpy(peerInfo.peer_addr, serverMac, 6);

  initESPNOW();

  analogReadResolution(12);

  pinMode(13, INPUT_PULLUP); 
  pinMode(32, INPUT_PULLUP); 
  pinMode(33, INPUT_PULLUP); 
  pinMode(25, INPUT_PULLUP); 
  pinMode(26, INPUT_PULLUP); 
  pinMode(27, INPUT_PULLUP); 
  pinMode(14, INPUT_PULLUP); 
  pinMode(12, INPUT_PULLUP); 
}
```

#### 主循环

在 `loop` 函数中，我们读取摇杆的模拟值和各个按钮的数字值，根据这些输入生成命令，并通过 ESP-NOW 发送到服务端：

```cpp
void loop() {
  if (!checkESPNOW()) {
    Serial.println("Reinitializing ESP-NOW");
    esp_now_deinit();
    delay(1000);
    initESPNOW();
  }

  int xValue = analogRead(34);
  int yValue = analogRead(35);
  int buttonState = digitalRead(13); 
  int buttonState32 = digitalRead(32);
  int buttonState33 = digitalRead(33); 
  int buttonState25 = digitalRead(25); 
  int buttonState26 = digitalRead(26); 
  int buttonState27 = digitalRead(27); 
  int buttonState14 = digitalRead(14); 
  int buttonState12 = digitalRead(12); 

  char command;

  if (yValue > 2048 + 500) {
    command = 'F';
  } else if (yValue < 2048 - 500) {
    command = 'B';
  } else if (xValue > 2048 + 500) {
    command = 'R';
  } else if (xValue < 2048 - 500) {
    command = 'L';
  } else {
    command = 'T';
  }

  if (buttonState == LOW) { 
    command = 'K'; 
  } else if (buttonState32 == LOW) { 
    command = 'Y'; 
    Serial.println("gpio32 Y");
  } else if (buttonState33 == LOW) { 
    command = 'G'; 
    Serial.println("gpio33 G");
  } else if (button

State25 == LOW) { 
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

  esp_now_send(serverMac, (uint8_t *)&command, sizeof(command));
  delay(100); 
}
```


通过上述代码，我们创建了一个基于 ESP-NOW 的无线控制器。该控制器可以读取摇杆和按钮的输入，并根据这些输入生成相应的命令发送给接收端。ESP-NOW 的快速、低功耗特点使其非常适合用于物联网设备之间的无线通信。希望本文能为你在物联网项目中使用 ESP-NOW 提供有用的参考。

## ESP-NOW 接收端设计与实现

