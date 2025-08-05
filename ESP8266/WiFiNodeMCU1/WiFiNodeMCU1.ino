/*************************************/
/*Mission14：NodeMCU+DHT11+LED接入aliyunIoT
//物联22409 孙浚杰 072240913
/*************************************/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <Ticker.h>
#include <Adafruit_SSD1306.h> 
#include <DHT.h>
#include <AliyunIoTSDK.h>
#include <SoftwareSerial.h>

// 设置wifi接入信息
const char* WiFi_SSID = "你的 WiFi 名称";
const char* WiFi_PASSWD = "密码";
const char* mqttServer = "MQTT连接参数的 Url";  //所创建设备的MQTT连接参数mqttHostUrl
 
//Ticker ticker;
WiFiClient wifiClient;  
PubSubClient mqttClient(wifiClient);

#define PIN_A A0      //光敏 ADC引脚A0
#define DHTPIN D3     //DHT 输出引脚D3
#define SMKPIN D1     //烟雾传感器

#define DHTTYPE  DHT11     // 定义传感器类型为DHT11 
DHT dht(DHTPIN, DHTTYPE);
float t = 0.0;//初始化温度变量
int h = 0;//初始化湿度变量
int L = 0;
int co2 = 0;
bool smoke = 0;
float comlv = 0; //   舒适度计算

unsigned long lastMsMain = 0;
// 设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY "阿里云产品密钥"
#define DEVICE_NAME "设备名"
#define DEVICE_SECRET "设备密码"
#define REGION_ID "cn-shanghai"//该参数不变

WiFiServer server(80);

// Auxiliar variables to store the current output state
String LedGreenState = "off";
String LedRedState = "off";

unsigned long currentTime = millis();// Current time
unsigned long previousTime = 0; // Previous time
const long timeoutTime = 2000;// Define timeout time in milliseconds (example: 2000ms = 2s)

// JW01新数据包结构
const byte PACKET_SIZE = 6;     // 6字节数据包
const byte DEVICE_ADDR = 0x2C;  // 设备地址固定为0x2C
// 调试标志
const bool DEBUG_MODE = true;
const int rxPin = 14;  // GPIO14 (D5) 连接传感器TX B
const int txPin = 2;   // GPIO2 (D4) 连接传感器RX  A
SoftwareSerial jw01Sensor(rxPin, txPin);

float calculate_comfort(float temperature, int humidity, int co2_value, int light_value);

void setup() {
  Serial.begin(115200);
  pinMode(DHTPIN,INPUT);
  pinMode(PIN_A, INPUT);
  pinMode(SMKPIN, OUTPUT);
  digitalWrite(SMKPIN, LOW);
  pinMode(SMKPIN, INPUT);
  //pinMode(LedRedPIN,OUTPUT);
  //pinMode(LedGreenPIN,OUTPUT);
  //digitalWrite(LedRedPIN, LOW);
  //digitalWrite(LedGreenPIN, LOW);   
  // 确保GPIO2在启动时保持高电平
  pinMode(txPin, OUTPUT);
  digitalWrite(txPin, HIGH);
  // 等待启动完成
  delay(1500); 
  jw01Sensor.begin(9600);
  Serial.println("JW01 CO2 Sensor initialized (6-byte format)");
  // 清空串口缓冲区
  while(jw01Sensor.available()) jw01Sensor.read();

  wifiInit();// 连接WiFi
  mqttClient.setServer(mqttServer, 1883); // 设置MQTT服务器和端口号
  connectMQTTServer();// 连接MQTT服务器
  dht.begin();  
  server.begin();    
  
//初始化 iot，需传入 wifi 的 client，和设备产品信息
  AliyunIoTSDK::begin(wifiClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);
  Serial.println("mission17 Start...");  
  //ticker.attach(1, tickerCount); // Ticker定时对象
  }  
 
 void loop() {
  //-----------------------------------------jw01------------------------------------------
  byte packet[PACKET_SIZE];
  int index = 0;
  bool foundHeader = false;
  unsigned long startTime = millis();
  // 接收数据包（带设备地址检测）
  while ((millis() - startTime < 1000) && (index < PACKET_SIZE)) {
    if (jw01Sensor.available()) {
      byte b = jw01Sensor.read();
      
      if (DEBUG_MODE) {
        Serial.print("0x");
        if (b < 0x10) Serial.print("0");
        Serial.print(b, HEX);
        Serial.print(" ");
      }
      
      // 检查设备地址（帧头）
      if (index == 0) {
        if (b == DEVICE_ADDR) {
          packet[index++] = b;
          foundHeader = true;
        }
      } 
      // 收集后续数据
      else if (foundHeader) {
        packet[index++] = b;
      }
    }
  }

  if (DEBUG_MODE) Serial.println();

  // 验证数据包完整性
  if (index != PACKET_SIZE) {
    Serial.print("Error: Incomplete packet (");
    Serial.print(index);
    Serial.println("/6 bytes)");
    
    // 显示接收到的部分数据
    if (DEBUG_MODE && index > 0) {
      Serial.print("Partial: ");
      for (int i = 0; i < index; i++) {
        Serial.print("0x");
        if (packet[i] < 0x10) Serial.print("0");
        Serial.print(packet[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
    return;
  }

  // 校验和计算（B1+B2+B3+B4+B5）
  byte checksum = 0;
  for (int i = 0; i < PACKET_SIZE - 1; i++) {
    checksum += packet[i];
  }

  // 校验和验证（B6）
  if (packet[5] != checksum) {
    Serial.print("Checksum error. Calc: 0x");
    Serial.print(checksum, HEX);
    Serial.print(" Recv: 0x");
    Serial.println(packet[5], HEX);
    return;
  }

  // 解析CO2值（B2和B3）
  co2 = (packet[1] << 8) | packet[2];
  
  // 输出结果
  Serial.print("CO₂: ");
  Serial.print(co2);
  Serial.println(" ppm");
  
  Serial.println("----------------------");
  //-----------------------------------------jw01------------------------------------------

  AliyunIoTSDK::loop(); 
  h = dht.readHumidity();
  t = dht.readTemperature();
  L = analogRead(PIN_A);
  L = 1024 - L;
  comlv = calculate_comfort(t,h,co2,L);
  if(digitalRead(SMKPIN) == LOW)
    smoke = 1;
  else
    smoke = 0;
  if (isnan(h) || isnan(t)) {
  Serial.println("Failed to read from DHT sensor!"); }
  Serial.print("temp: "); Serial.print(t); Serial.println("°C");
  Serial.print("humi: "); Serial.print(h);  Serial.println("%");
  Serial.print("light: "); Serial.print(L);  Serial.println(" lux");
  Serial.print("smoke: "); Serial.print(smoke); 

      AliyunIoTSDK::send("temp",t);//发送当前温度值到云平台
      AliyunIoTSDK::send("humi",h);//发送当前湿度值到云平台
      AliyunIoTSDK::send("light",L);//发送当前guang度值到云平台
      AliyunIoTSDK::send("co2",co2);//发送当前co2/ppm值到云平台
      AliyunIoTSDK::send("comfort",comlv);//发送当前舒适度值到云平台
      AliyunIoTSDK::send("smoke",smoke);//烟雾状态

   WiFiClient client = server.available();   // Listen for incoming clients

 delay(1500) ;
}  

void connectMQTTServer(){  //连接mqtt服务器
  //根据设备信息中的MQTT连接参数，替换下面字符串内容
  String clientId = "k1ozvJXrRxK.THControllor|securemode=2,signmethod=hmacsha256,timestamp=1725237470631|";   
  String user = "THControllor&k1ozvJXrRxK";
  String password = "3ef27d2ffb564484c0ef244afd434f878bba64a3d4c4fc973308b1b0a932a3da";
 
  if (mqttClient.connect(clientId.c_str(),user.c_str(),password.c_str())) { 
    Serial.println("MQTT Server Connected.");
    Serial.println("Server Address: ");
    Serial.println(mqttServer);
    Serial.println("ClientId:");
    Serial.println(clientId); } 
    else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(3000); }   
}

// NodeMCU连接wifi
void wifiInit(){
    WiFi.mode(WIFI_STA); //设置ESP8266工作模式为无线终端模式
    WiFi.begin(WiFi_SSID, WiFi_PASSWD);   
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("WiFi not Connect"); }
    Serial.print("NodeMCU Connected to ");
    Serial.println(WiFi.SSID());  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); 
    }

// 舒适度计算函数
float calculate_comfort(float temperature, int humidity, int co2_value, int light_value){
    // 1. 温度舒适度计算 (35%权重)
    float temp_score;
    if (temperature >= 22.0 && temperature <= 24.0) {
        temp_score = 100.0;                               // 最佳温度区间 22-24
    } else if (temperature >= 20.0 && temperature < 22.0) {
        temp_score = 90.0 - (22.0 - temperature) * 10.0;  // 每降低0.1°C扣1分
    } else if (temperature > 24.0 && temperature <= 26.0) {
        temp_score = 90.0 - (temperature - 24.0) * 10.0;  // 每升高0.1°C扣1分
    } else if (temperature >= 18.0 && temperature < 20.0) {
        temp_score = 70.0 - (20.0 - temperature) * 5.0;  // 每降低0.2°C扣1分
    } else if (temperature > 26.0 && temperature <= 28.0) {
        temp_score = 70.0 - (temperature - 26.0) * 5.0;  // 每升高0.2°C扣1分
    } else if (temperature < 18.0) {
        temp_score = 30.0 - (18.0 - temperature);  // 每降低1°C扣1分
    } else {
        temp_score = 30.0 - (temperature - 28.0);  // 每升高1°C扣1分
    }
    if (temp_score < 0.0) temp_score = 0.0;
    
    // 2. 湿度舒适度计算 (25%权重)
    float hum_score;
    float humidity_f = (float)humidity; // 转换为浮点数
    if (humidity_f >= 40.0 && humidity_f <= 60.0) {
        hum_score = 100.0;                                 // 最佳湿度区间  40-60
    } else if (humidity_f >= 30.0 && humidity_f < 40.0) {
        hum_score = 90.0 - (40.0 - humidity_f) * 2.0;  // 每降低1%扣2分
    } else if (humidity_f > 60.0 && humidity_f <= 70.0) {
        hum_score = 90.0 - (humidity_f - 60.0) * 2.0;  // 每升高1%扣2分
    } else if (humidity_f >= 20.0 && humidity_f < 30.0) {
        hum_score = 70.0 - (30.0 - humidity_f) * 2.0;  // 每降低1%扣2分
    } else if (humidity_f > 70.0 && humidity_f <= 80.0) {
        hum_score = 70.0 - (humidity_f - 70.0) * 2.0;  // 每升高1%扣2分
    } else if (humidity_f < 20.0) {
        hum_score = 30.0 - (20.0 - humidity_f) * 1.5;  // 每降低1%扣1.5分
    } else {
        hum_score = 30.0 - (humidity_f - 80.0) * 1.5;  // 每升高1%扣1.5分
    }
    if (hum_score < 0.0) hum_score = 0.0;
    
    // 3. 空气质量计算 (25%权重) - 针对350-1023ppm范围优化
    float air_score;
    if (co2_value < 450.0) {
        air_score = 100.0;                                  // 极佳空气质量 <450
    } else if (co2_value < 600.0) {
        air_score = 90.0 + (600.0 - co2_value) * 0.1;  // 每降低10ppm加1分
    } else if (co2_value < 800.0) {
        air_score = 90.0 - (co2_value - 600.0) * 0.125;  // 每增加8ppm扣1分
    } else if (co2_value < 1000.0) {
        air_score = 80.0 - (co2_value - 800.0) * 0.1;  // 每增加10ppm扣1分
    } else {
        air_score = 60.0 - (co2_value - 1000.0) * 2.6087;  // 在1000-1023范围内线性降至30分
    }
    if (air_score < 0.0) air_score = 0.0;
    
    // 4. 光照舒适度 (15%权重) - 针对0-1000 lux范围优化
    float light_score;
    // 将ADC值转换为lux (假设0-1023对应0-1000 lux)
    float light_lux = map(light_value, 0, 1023, 0, 1000);
    
    if (light_lux < 50.0) {
        light_score = 30.0;  // 非常暗
    } else if (light_lux < 100.0) {
        light_score = 50.0;  // 较暗
    } else if (light_lux < 300.0) {
        // 线性从50分到90分 (100-300 lux)
        light_score = 50.0 + (light_lux - 100.0) * 0.2;  // 每增加10lux增加2分
    } else if (light_lux < 750.0) {
        // 最佳光照区间                                         (300-750 lux)
        light_score = 90.0 + (light_lux - 300.0) * 0.0222;  // 缓慢增加至100分
    } else if (light_lux <= 1000.0) {
        // 较亮 (750-1000 lux)
        light_score = 100.0 - (light_lux - 750.0) * 0.12;  // 每增加20lux扣2.4分
    } else {
        // 超过1000 lux按1000 lux计算
        light_score = 70.0;
    }
    
    // 确保光照分数在0-100范围内
    if (light_score < 0.0) light_score = 0.0;
    if (light_score > 100.0) light_score = 100.0;
    
    // 5. 综合评分 (加权平均)
    float comfort_score = (
        0.35 * temp_score + 
        0.25 * hum_score + 
        0.25 * air_score + 
        0.15 * light_score
    );
    
    // 确保总分在0-100范围内
    if (comfort_score < 0.0) comfort_score = 0.0;
    if (comfort_score > 100.0) comfort_score = 100.0;
    
    return comfort_score;
}
