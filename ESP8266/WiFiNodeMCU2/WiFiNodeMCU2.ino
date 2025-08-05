/*************************************/
/* Mission14：Node2 数据接收端 (直接使用PubSubClient)
   物联22409 孙浚杰 072240913
/*************************************/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// 设置WiFi信息
const char* WiFi_SSID = "你的 WiFi 名称";
const char* WiFi_PASSWD = "密码";

// MQTT服务器信息
const char* mqttServer = "iot-06z00e9cmifyqci.mqtt.iothub.aliyuncs.com";
const int mqttPort = 1883;

// 节点2的设备信息
#define PRODUCT_KEY "密钥"
#define DEVICE_NAME "设备名"
#define DEVICE_SECRET "密码"

// 云流转消息Topic
const char* CLOUD_FORWARD_TOPIC = "用于接收云消息的 topic";
// 执行器状态更新
const char* USER_UPDATE_TOPIC = "节点发送设备状态到的topic";

// 存储接收到的数据
float receivedTemp = 0.0;
int receivedHumi = 0;
int receivedCO2 = 0;
int receivedLight = 0;  // 添加光照强度变量
bool receivedsmoke = 0;

// 设备状态变量（在文件顶部添加）
int acStatus = 0;          // 0=关, 1=制冷, 2=制热
bool buzzerStatus = 0;
bool relayStatus = 0;
bool humStatus = 0;
bool lightStatus = 0;
bool modeStatus = 0;

char cmd[1024] = {0};

/*
#define FAN_PIN D1
#define HUMIDIFIER_PIN 2
#define AC_POWER_PIN 3
#define MODE_LED_PIN 4
#define LIGHT_PIN D2
#define BUZZ_PIN D0
*/

#define SOFT_RX D3 // 接收引脚 
#define SOFT_TX D4 // 发送引脚
SoftwareSerial softSerial(SOFT_RX, SOFT_TX); // 创建软件串口对象

void publishDeviceStatus();
WiFiClient wiFiClient;
PubSubClient mqttClient(wiFiClient);

void setup() {
  Serial.begin(115200);
  softSerial.begin(115200); // 初始化软件串口

  //pinMode(FAN_PIN,OUTPUT);
  //pinMode(BUZZ_PIN,OUTPUT);
  // 初始化WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFi_SSID, WiFi_PASSWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());

  // 设置MQTT服务器和回调
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(cloudForwardCallback);
  
  // 连接MQTT服务器
  connectMQTTServer();

  // 初始化设备状态
  acStatus = 0;
  buzzerStatus = false;
  relayStatus = false;
  //smokeStatus = false;

  Serial.println("Node2 数据接收端启动");
  Serial.println("等待来自节点1的数据...");
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTTServer();
  }
  mqttClient.loop();

  displaySensorData();   // 显示传感器数据
  
  //softSerial.println("66666666666你好666666666666666666666666");
  // 串口获取到指令
  if (softSerial.available()) {
    String command = softSerial.readStringUntil('\n');
    command.trim();// 用于去除字符串两端的空白字符（包括空格、制表符、换行符等）。
    Serial.print("从软件串口收到指令: ");
    Serial.println(command);
    updateStatus(command.c_str());// 这个方法将String对象转换为C风格字符串（即一个以空字符`\0`结尾的字符数组）。

    //softSerial.println("收到啦"); // 串口发送
    //delay(1500);
  }

}
// 执行器状态变量更新函数
void updateStatus(const char* command) {
  Serial.print("---------------------------\n");
  //Serial.print("执行器状态更新: ");
  //Serial.println(command);
  
  if (strcmp(command, "9101") == 0) {// 10风扇 1开
    relayStatus = 1;
    Serial.println("风扇状态更新-已开启");
  }
  else if (strcmp(command, "9100") == 0) {// 10风扇 0关
    relayStatus = 0;
    Serial.println("风扇状态更新-已关闭");
  }
  else if (strcmp(command, "201") == 0) {
    humStatus = 1;
    Serial.println("加湿器状态更新-已开启");
  }
  else if (strcmp(command, "200") == 0) {
    humStatus = 0;
    Serial.println("加湿器状态更新-已关闭");
  }
  else if (strcmp(command, "600") == 0) {
    acStatus = 0;
    Serial.println("空调状态更新-已关闭");
  }
  else if (strcmp(command, "601") == 0) {
    acStatus = 1;
    Serial.println("空调状态更新-制冷");
  }
  else if (strcmp(command, "602") == 0) {
    acStatus = 2;
    Serial.println("空调状态更新-制热");
  }
  else if (strcmp(command, "301") == 0) {
    modeStatus = 1;
    Serial.println("状态更新-切换到自动模式");
  }
  else if (strcmp(command, "300") == 0) {
    modeStatus = 0;
    Serial.println("状态更新-切换到手动模式");
  }
  else if (strcmp(command, "9401") == 0) {
    lightStatus = 1;
    Serial.println("补光灯状态更新-已开启");
  }
  else if (strcmp(command, "9400") == 0) {
    lightStatus = 0;
    Serial.println("补光灯状态更新-已关闭");
  }
  else if (strcmp(command, "501") == 0) {
    buzzerStatus = 1;
    Serial.println("蜂鸣器状态更新-已打开");
  }
  else if (strcmp(command, "500") == 0) {
    buzzerStatus = 0;
    Serial.println("蜂鸣器状态更新-已关闭");
  }
  else {
    Serial.println("状态更新-未知指令");
  }
  Serial.print("---------------------------\n");
}

// 指令解析与执行函数
void executeCommand(const char* command) {
  //Serial.print("执行指令: ");
  //Serial.println(command);
  
  if (strcmp(command, "9101") == 0) {//910风扇 1开
    softSerial.println(command);
    Serial.println("指令发送-风扇已开启");
  }
  else if (strcmp(command, "9100") == 0) {//910风扇 0关
    softSerial.println(command);
    Serial.println("指令发送-风扇已关闭");
  }
  else if (strcmp(command, "201") == 0) {
    softSerial.println(command);
    Serial.println("指令发送-加湿器已开启");
  }
  else if (strcmp(command, "200") == 0) {
    softSerial.println(command);
    Serial.println("指令发送-加湿器已关闭");
  }
  else if (strcmp(command, "600") == 0) {
    softSerial.println(command);
    Serial.println("指令发送-空调已关闭");
  }
  else if (strcmp(command, "601") == 0) {
    softSerial.println(command);
    Serial.println("指令发送-空调制冷模式开启");
  }
  else if (strcmp(command, "602") == 0) {
    softSerial.println(command);
    Serial.println("指令发送-空调制热模式开启");
  }
  else if (strcmp(command, "301") == 0) {
    softSerial.println(command);
    Serial.println("指令发送-切换到自动模式");
  }
  else if (strcmp(command, "300") == 0) {
    softSerial.println(command);
    Serial.println("指令发送-切换到手动模式");
  }
  else if (strcmp(command, "9401") == 0) {
    softSerial.println(command);
    Serial.println("指令发送-补光灯已开启");
  }
  else if (strcmp(command, "9400") == 0) {
    softSerial.println(command);
    Serial.println("指令发送-补光灯已关闭");
  }
  else if (strcmp(command, "501") == 0) {
    softSerial.println(command);
    Serial.println("指令发送-蜂鸣器已打开");
  }
  else if (strcmp(command, "500") == 0) {
    softSerial.println(command);
    Serial.println("指令发送-蜂鸣器已关闭");
  }
  else {
    Serial.println("指令转发-未知指令");
  }
  memset(cmd,0x00,sizeof(cmd));
}
// 发布设备状态到用户更新Topic
void publishDeviceStatus() {
  // 创建JSON文档
  StaticJsonDocument<256> doc;
  
  // 设置固定字段
  //doc["id"] = String(millis()); // 使用时间戳作为消息ID
  doc["id"] = "123";
  doc["version"] = "1.0";
  doc["method"] = "thing.event.property.post";
  JsonObject params = doc.createNestedObject("params");
    // 传感器数据
  params["temp"] = receivedTemp; //1
  params["humi"] = receivedHumi; //1
  params["co2"] = receivedCO2;   //2
  params["light"] = receivedLight;//3
  params["smoke"] = receivedsmoke ? 1 : 0;//4
  // 设置设备状态参数
  params["AC"] = acStatus;       // 枚举类型                   1
  params["Buzzer"] = buzzerStatus ? 1 : 0; // 布尔类型转0/1    2
  params["Relay"] = relayStatus ? 1 : 0; //                   3
  params["Hum"] = humStatus ? 1 : 0; //                       4
  params["LEDlight"] = lightStatus ? 1 : 0; //                5
  params["mode"] = modeStatus ? 1 : 0; //                     6
  // 序列化JSON
  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);
  
  // 发布到用户更新Topic
  if (mqttClient.publish(USER_UPDATE_TOPIC, jsonBuffer)) {
    //Serial.println("设备状态上报成功");
    //Serial.print("Topic: ");
    //Serial.println(USER_UPDATE_TOPIC);
    //Serial.print("内容: ");
    //Serial.println(jsonBuffer);
  } else {
    Serial.println("设备状态上报失败!");
  }
}

// 连接MQTT服务器
void connectMQTTServer() {
  // 生成MQTT连接参数
  String clientId = "k1ozvJXrRxK.ACTControllor|securemode=2,signmethod=hmacsha256,timestamp=1750233713332|";
  String username = "ACTControllor&k1ozvJXrRxK";
  
  // 计算密码 - 需要根据阿里云规则计算HMAC-SHA256
  String password = "24aec4dcf0d47cd175738547eae0616062a6e1065d523258cfc870d1b05149be"; // 请替换为实际密码

  // 尝试连接
  while (!mqttClient.connected()) {
    Serial.println("尝试连接MQTT服务器...");
    if (mqttClient.connect(clientId.c_str(), username.c_str(), password.c_str())) {
      Serial.println("MQTT服务器连接成功");
      // 订阅云流转Topic
      mqttClient.subscribe(CLOUD_FORWARD_TOPIC);
      Serial.print("已订阅Topic: ");
      Serial.println(CLOUD_FORWARD_TOPIC);
    } else {
      Serial.print("连接失败, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" 5秒后重试...");
      delay(5000);
    }
  }
}

// 云流转消息回调函数
void cloudForwardCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("收到云流转消息: ");
  //Serial.println(topic);
  
  // 将payload转为字符串
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  
  //Serial.print("原始数据: ");
  Serial.println(message);
  
  // 解析JSON数据
  DynamicJsonDocument doc(1024);  // 增加缓冲区大小
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.print("JSON解析失败: ");
    Serial.println(error.c_str());
    return;
  }
  
  // 提取节点1的数据
  if (doc.containsKey("items")) {
    JsonObject items = doc["items"];
    
    // 提取温度值
    if (items.containsKey("temp")) {
      receivedTemp = items["temp"]["value"].as<float>();
      Serial.print("解析温度: ");
      Serial.print(receivedTemp);
      Serial.println("°C");
    }
    
    // 提取湿度值
    if (items.containsKey("humi")) {
      receivedHumi = items["humi"]["value"].as<int>();
      Serial.print("解析湿度: ");
      Serial.print(receivedHumi);
      Serial.println("%");
    }
    
    // 提取CO2值
    if (items.containsKey("co2")) {
      receivedCO2 = items["co2"]["value"].as<int>();
      Serial.print("解析CO2: ");
      Serial.print(receivedCO2);
      Serial.println("ppm");
    }
    
    // 提取光照值
    if (items.containsKey("light")) {
      receivedLight = items["light"]["value"].as<int>();
      Serial.print("解析光照: ");
      Serial.print(receivedLight);
      Serial.println(" lux");
    }
    
    // 提取烟雾开关状态
    if (items.containsKey("smoke")) {
      receivedsmoke = items["smoke"]["value"].as<bool>();
      Serial.print("解析烟雾: ");
      Serial.print(receivedsmoke);
      Serial.println(" ");
    }

    Serial.println("----------------------");
  } else {
    strcpy(cmd,message);
    Serial.println("数据格式错误: 缺少'items'字段,检查是否为指令：");
    executeCommand(cmd);
  }
}

// 显示传感器数据
void displaySensorData() {
  static unsigned long lastDisplay = 0;
  if (millis() - lastDisplay > 2000) { // 每2秒
    lastDisplay = millis();

    //Serial.println("\n===== 最新传感器数据 =====");
    //Serial.print("温度: "); Serial.print(receivedTemp); Serial.println("°C");
    //Serial.print("湿度: "); Serial.print(receivedHumi); Serial.println("%");
    //Serial.print("CO₂:  "); Serial.print(receivedCO2); Serial.println("ppm");
    //Serial.print("光照: "); Serial.print(receivedLight); Serial.println(" lux");
    //Serial.print("烟雾传感器状态: "); Serial.print(receivedsmoke); Serial.println(" ");
    //Serial.println("========================");

    // 创建要发送的数据字符串
    char dataToSend[32]; // 足够大的缓冲区
    // 格式化字符串: T温度 H湿度 L光照 CCO2 S烟雾
    snprintf(dataToSend, sizeof(dataToSend), "DA:T%.1fH%dL%dC%dS%d",
             receivedTemp, 
             receivedHumi, 
             receivedLight, 
             receivedCO2, 
             receivedsmoke ? 1 : 0);  // 将布尔值转换为0/1
    //Serial.print("串口发送: ");Serial.println(dataToSend);
    softSerial.println(dataToSend);//数据发送给STM32

    publishDeviceStatus();
  }
}
