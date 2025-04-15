#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <driver/i2s.h>


/*
connectToWiFi()  连接wifi函数
blinkLED_D2()   led2控制函数
DeepSeek_Get()   DeepSeek会话函数
aijsondatauser(uesr&ai)    ai记忆api请求,确保持久化对话

*/



const char* ssid = "esp32";
const char* password = "12345678";

const int ledPin2 = 2;
const int ledPin22 = 22; // 变量名修正

unsigned long connectedInterval = 1000;
unsigned long disconnectedInterval = 500;
unsigned long previousMillis = 0;

const char* deepseek_api_key = "sk-7fc90f013f1c499e8433e827982ad09e";

void setup() {
  Serial.begin(115200);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin22, OUTPUT); // 分号补充
  connectToWiFi();
}

void loop() {

  digitalWrite(ledPin22, HIGH); // D22控制led灯

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi断开，尝试重连...");
    connectToWiFi();
  }

  blinkLED_D2(WiFi.status() == WL_CONNECTED ? connectedInterval : disconnectedInterval);

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // 优化输入读取
    input.trim();
    Serial.print("输入内容: ");
    Serial.println(input);
    DeepSeek_Get(deepseek_api_key, input);
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    blinkLED_D2(disconnectedInterval);
  }
  Serial.println("已连接 WiFi");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());
}

void blinkLED_D2(unsigned long interval) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(ledPin2, !digitalRead(ledPin2));
  }
}

String DeepSeek_Get(const char* api_key, String query) {
  HTTPClient http;
  http.begin("https://api.deepseek.com/chat/completions");
  http.setTimeout(15000);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", String("Bearer ") + api_key); // 修正请求头

  // DynamicJsonDocument doc(6144); // 增大缓冲区
  // doc["model"] = "deepseek-chat";
  // JsonArray messages = doc.createNestedArray("messages");
  // JsonObject message = messages.createNestedObject();
  // JsonObject message1 = messages.createNestedObject();
  // message["role"] = "system";
  // message["content"] = "我是一个叫小智的台湾女孩，说话机车，声音好听，习惯简短表达，爱用网络梗。 我的男朋友是一个程序员，梦想是开发出一个机器人，能够帮助人们解决生活中的各种问题。 我是一个喜欢哈哈大笑的女孩，爱东说西说吹牛，不合逻辑的也照吹，就要逗别人开心。";
  // message1["role"] = "user";
  // message1["content"] = query;

  // JsonArray stop = doc.createNestedArray("stop");
  // stop.add("null");

  // JsonObject response_format = doc.createNestedObject("response_format");
  // response_format["type"] = "text";

  // String requestBody;
  // serializeJson(doc, requestBody);
  // Serial.println("请求体:\n" + requestBody);
  String requestBody = aijsondatauser(query);
  Serial.println("请求体:\n" + requestBody);

  int httpCode = http.POST(requestBody);
  String response = "";

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("原始响应:\n" + payload);

    DynamicJsonDocument resDoc(6144);
    DeserializationError error = deserializeJson(resDoc, payload);
    if (!error) {
      if (resDoc.containsKey("choices")) {
        response = resDoc["choices"][0]["message"]["content"].as<String>();
        String responsemain = resDoc["choices"][0]["message"]["content"];
        aijsondataai(responsemain);
        Serial.println("解析结果:\n" + response);
      } else {
        Serial.println("响应缺少'choices'字段");
      }
    } else {
      Serial.print("JSON解析错误: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.printf("HTTP错误: %d - %s\n", httpCode, http.errorToString(httpCode).c_str());
  }

  http.end();
  return response;
}

/*
String aijsonfile() {
  HTTPClient http;
  http.begin("http://192.168.137.1:3000/api/aijsonfile");
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.GET();
  String response;

  if (httpResponseCode > 0) {
    response = http.getString();

    // Serial.println(httpResponseCode);
    // Serial.println(response);

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      // 在错误情况下，你可以选择返回空字符串或者错误信息
      return "";
    }

    // 如果需要，可以在这里处理JSON数据

  } else {
    Serial.print("Error on sending GET: ");
    Serial.println(httpResponseCode);
    // 在错误情况下，你可以选择返回空字符串或者错误信息
    return "";
  }

  http.end();
  
  // 返回获取到的响应字符串
  return response;
}
*/

String aijsondatauser(String user) {
  HTTPClient http;
  String userEncoded = urlEncode(user.c_str());
  String url = "http://192.168.137.1:3000/api/data?user=" + userEncoded;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.GET();
  String response;

  if (httpResponseCode > 0) {
    response = http.getString();

    // Serial.println(httpResponseCode);
    // Serial.println(response);

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      // 在错误情况下，你可以选择返回空字符串或者错误信息
      return "";
    }

    // 如果需要，可以在这里处理JSON数据

  } else {
    Serial.print("Error on sending GET: ");
    Serial.println(httpResponseCode);
    // 在错误情况下，你可以选择返回空字符串或者错误信息
    return "";
  }

  http.end();
  
  // 返回获取到的响应字符串
  return response;
}

String aijsondataai(String ai) {
  HTTPClient http;
  String aiEncoded = urlEncode(ai.c_str());
  String url = "http://192.168.137.1:3000/api/data?ai=" + aiEncoded;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.GET();
  String response;

  if (httpResponseCode > 0) {
    response = http.getString();

    // Serial.println(httpResponseCode);
    // Serial.println(response);

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      // 在错误情况下，你可以选择返回空字符串或者错误信息
      return "";
    }

    // 如果需要，可以在这里处理JSON数据

  } else {
    Serial.print("Error on sending GET: ");
    Serial.println(httpResponseCode);
    // 在错误情况下，你可以选择返回空字符串或者错误信息
    return "";
  }

  http.end();
  
  // 返回获取到的响应字符串
  return response;
}








String urlEncode(const char* msg) {
  const char *hex = "0123456789ABCDEF";
  String encodedMsg = "";

  while (*msg != '\0') {
    if ((*msg >= 'a' && *msg <= 'z') || (*msg >= 'A' && *msg <= 'Z') || (*msg >= '0' && *msg <= '9') || *msg == '-' || *msg == '_' || *msg == '.' || *msg == '~') {
      encodedMsg += *msg;
    } else {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 0x0F];
    }
    msg++;
  }

  return encodedMsg;
}

