#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>  // สำหรับจอ OLED 1.3"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ==== กำหนดขาเซ็นเซอร์ฝุ่น ====
#define SENSOR_LED 4       // ขา LED ของ GP2Y1014AU0F
#define SENSOR_VO 34       // ขา Output (Vo)

// ==== ค่ามาตรฐานเซ็นเซอร์ ====
const float K = 0.17;  // ค่าความชัน (โวลต์ -> ความหนาแน่นฝุ่น)
const float V0 = 0.35; // ค่าแรงดันตอนอากาศสะอาด (โวลต์)

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA=21, SCL=22

  pinMode(SENSOR_LED, OUTPUT);
  pinMode(SENSOR_VO, INPUT);

  // เริ่มต้นจอ OLED
  if(!display.begin(0x3C, true)) {
    Serial.println(F("ไม่พบจอ OLED"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Dust Sensor Starting...");
  display.display();
  delay(1500);
}

void loop() {
  // เปิด LED เพื่ออ่านค่า
  digitalWrite(SENSOR_LED, LOW);
  delayMicroseconds(280);
  int sensorValue = analogRead(SENSOR_VO);
  delayMicroseconds(40);
  digitalWrite(SENSOR_LED, HIGH);
  delayMicroseconds(9680);

  // แปลงค่า ADC เป็นแรงดัน (0–3.3V)
  float voltage = sensorValue * (3.3 / 4095.0);

  // แปลงแรงดันเป็นค่าฝุ่น (ug/m3)
  float dustDensity = (voltage - V0) / K;
  if (dustDensity < 0) dustDensity = 0;

  // หาระดับคุณภาพอากาศ (AQI)
  String airLevel;
  if (dustDensity <= 50) airLevel = "ดีมาก";
  else if (dustDensity <= 100) airLevel = "ปานกลาง";
  else if (dustDensity <= 150) airLevel = "เริ่มมีผล";
  else if (dustDensity <= 200) airLevel = "มีผลต่อสุขภาพ";
  else if (dustDensity <= 300) airLevel = "รุนแรง";
  else airLevel = "อันตราย!";

  // แสดงผลใน Serial Monitor
  Serial.print("Analog: "); Serial.print(sensorValue);
  Serial.print(" | Voltage: "); Serial.print(voltage, 3);
  Serial.print(" V | Dust: "); Serial.print(dustDensity, 1);
  Serial.print(" ug/m3 | Level: "); Serial.println(airLevel);

  // แสดงบนจอ OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Dust Sensor");

  display.setTextSize(2);
  display.setCursor(0,16);
  display.print(dustDensity, 0);
  display.println(" ug/m3");

  display.setTextSize(1);
  display.setCursor(0,45);
  display.print("Level: ");
  display.println(airLevel);
  display.display();

  delay(1000);
}