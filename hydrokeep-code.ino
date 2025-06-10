// ---------------------
// 🔧 CONFIG: ตั้งค่าต่าง ๆ
// ---------------------
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// 🟡 WiFi Settings
const char* ssid = "Your_SSID";               // <-- ใส่ชื่อ WiFi
const char* password = "Your_WIFI_PASSWORD";  // <-- ใส่รหัส WiFi

// 🟢 LINE Messaging API
String accessToken = "Bearer YOUR_LINE_TOKEN";  // <-- ใส่ LINE TOKEN
String userId = "YOUR_LINE_USER_ID";            // <-- ใส่ UserID (หรือ GroupID)

// 🔵 Google Chat Webhook
String googleChatWebhook = "https://chat.googleapis.com/v1/spaces/..."; // <-- ใส่ Google Chat Webhook URL

// 🟣 LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, จอขนาด 16x2

// 🔴 Pressure Sensor Pin
const int pressurePin = 34; // Analog input pin

// ⏰ NTP Time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000); // GMT+7

// ---------------------
// 📏 ขนาดบ่อจริง (สี่เหลี่ยมคางหมู)
// ---------------------
const float base1 = 15.66; // ด้านบน
const float base2 = 15.70; // ด้านล่าง
const float height = (11.73 + 16.18) / 2; // ด้านเฉียงเฉลี่ย
const float maxDepth = 2.5; // ความลึกบ่อสูงสุด

const float maxVolume = 0.5 * (base1 + base2) * height * maxDepth * 1000.0; // ลิตร

// ---------------------
// 🔔 การแจ้งเตือน
// ---------------------
bool alreadyReportedToday = false;
unsigned long lastLowAlertMillis = 0;
const unsigned long alertInterval = 12UL * 60UL * 60UL * 1000UL; // 12 ชั่วโมง

// ---------------------
void setup() {
  Serial.begin(115200);
  Wire.begin();
  lcd.begin(16, 2);
  lcd.backlight();

  pinMode(pressurePin, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");

  timeClient.begin();
}

// ---------------------
void loop() {
  timeClient.update();
  int hourNow = timeClient.getHours();
  int minuteNow = timeClient.getMinutes();
  unsigned long nowMillis = millis();

  // 🔄 อ่านค่า Sensor
  int raw = analogRead(pressurePin);
  float voltage = raw * (3.3 / 4095.0);
  float pressure_kPa = map(voltage * 1000, 500, 4500, 0, 100); // mV → kPa
  float waterHeight = pressure_kPa / 9.81;
  if (waterHeight > maxDepth) waterHeight = maxDepth;

  // คำนวณปริมาณน้ำ
  float volume = 0.5 * (base1 + base2) * height * waterHeight * 1000.0; // ลิตร
  float percent = (volume / maxVolume) * 100.0;

  // 📺 แสดงผล LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water status: ");
  lcd.print(percent < 10 ? "LOW" : "OK ");
  lcd.setCursor(0, 1);
  lcd.print("Amount: ");
  lcd.print((int)volume);
  lcd.print(" L");

  // 📅 รายงาน 6 โมงเช้า
  if (hourNow == 6 && minuteNow == 0 && !alreadyReportedToday) {
    String msg = "📊 รายงานประจำวันที่ " + String(hourNow) + " น.\n";
    msg += "💧 ระดับน้ำ: " + String((int)percent) + "%\n";
    msg += "🔢 ปริมาณ: " + String((int)volume) + " ลิตร\n";
    msg += "✅ สถานะ: " + String(percent < 10 ? "⚠️ ต่ำ" : "ปกติ");
    sendLineMessage(msg);
    sendGoogleChatMessage(msg);
    alreadyReportedToday = true;
  }

  // เคลียร์ flag 7 โมง
  if (hourNow == 7) alreadyReportedToday = false;

  // แจ้งเตือนทันทีหากน้ำน้อยกว่า 10%
  if (percent < 10.0 && nowMillis - lastLowAlertMillis > alertInterval) {
    String warnMsg = "🚨 แจ้งเตือน! ระดับน้ำต่ำ\n";
    warnMsg += "💧 เหลือเพียง: " + String((int)percent) + "%\n";
    warnMsg += "🔢 ปริมาณ: " + String((int)volume) + " ลิตร";
    sendLineMessage(warnMsg);
    sendGoogleChatMessage(warnMsg);
    lastLowAlertMillis = nowMillis;
  }

  delay(10000); // เช็คทุก 10 วินาที
}

// ---------------------
// 📤 LINE
// ---------------------
void sendLineMessage(String message) {
  HTTPClient http;
  http.begin("https://api.line.me/v2/bot/message/push");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", accessToken);

  String body = "{\"to\":\"" + userId + "\",\"messages\":[{\"type\":\"text\",\"text\":\"" + message + "\"}]}";
  http.POST(body);
  http.end();
}

// ---------------------
// 📤 Google Chat
// ---------------------
void sendGoogleChatMessage(String msg) {
  HTTPClient http;
  http.begin(googleChatWebhook);
  http.addHeader("Content-Type", "application/json");

  String json = "{\"text\": \"" + msg + "\"}";
  http.POST(json);
  http.end();
}
