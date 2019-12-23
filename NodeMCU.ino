#include <ESP8266WiFi.h>  //NodeMCU必备头文件
#include <DHT.h>  //温湿度传感器头文件

#define DHTPIN 12                         //GPIO12口，即D6口
#define DHTTYPE DHT11                     //DHT 11 温湿度传感器

const char* ssid     = "Galaxy Note 8";     //移动热点    
const char* password = "12345678";     //移动热点密码
const char* host = "api.heclouds.com";  //主机URL
const char* APIKEY = "lD4wpiYUSrMrzA3jL3IFkY2he3s=";    // API KEY

int32_t deviceId = 561908822;                             // Device ID
const char* DS_Temp = "Temperature";                        // 字符指针，数据流 - Temp
const char* DS_Hum = "Humidity";                          // 字符指针，数据流 - Hum               
const unsigned long HTTP_TIMEOUT = 2100;                // 最长响应时间

WiFiClient client;  //从端
const int tcpPort = 80; //80端口

DHT dht(DHTPIN, DHTTYPE); //DHT温湿度传感器库函数(引脚，传感器型号)

void setup() {
  WiFi.mode(WIFI_STA);                 //STATION 路由模式
  Serial.begin(115200);
  delay(10);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  //连接WiFi网络

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  //未连接，持续输出"."
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //输出本机IP地址

  dht.begin();  //温湿度传感器开始工作
}

void loop() {
  static float k = 0; //调试变量,作为a*sin(ωk+b)+c的参数
  delay(1000); //每一秒读取一次
  k+=0.5; //正弦曲线的振动，检测是否在动态读取数据
  float h = dht.readHumidity(); //真实变量，读取湿度
  //float h = 25 + 4* sin(k);
  float t = dht.readTemperature(); //真实变量，读取温度
  //float t = 50 - 5 * sin(k);
  Serial.print("tempreture:");
  Serial.print(t);
  Serial.print("\t humidity:");
  Serial.println(h);

  if (isnan(h) || isnan(t)) {  //若无法读取温度或湿度(Not A Number)，将不会发送，此后继续尝试读取
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (!client.connect(host, tcpPort)) {
    Serial.println("connection failed"); //若未建立TCP连接，将不会发送，此后重新进行循环
    return;
  }
  //传输
  postData(deviceId, t, h); 


  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 2000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;  //超时2000毫秒，释放连接
    }
  }
  Serial.println("closing connection");
}

void postData(int dId, float val_t, float val_h) { //定义数据传输方法
  //创建URI
  String url = "/devices/";
  url += String(dId); //设备编号
  url += "/datapoints?type=3";      //数据点
  String data = "{\"" + String(DS_Temp) + "\":" + String(val_t) + ",\"" +
                String(DS_Hum) + "\":" + String(val_h) + "}";  //拼接JSON串
  Serial.println(data);
  Serial.print("data length:");
  Serial.println(String(data.length()));
  String post_data = "POST " + url + " HTTP/1.1\r\n" +
                     "api-key:" + APIKEY + "\r\n" +
                     "Host:" + host + "\r\n" +  
                     "Content-Length: " + String(data.length()) + "\r\n" +        //发送数据长度
                     "Connection: close\r\n\r\n" +
                     data;  //JSON串加入URI
  Serial.println(post_data);
}
