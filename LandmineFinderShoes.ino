#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

// -------- Pin Configuration --------
#define GPS_RX      16     // ESP32 RX  (GPS TX)
#define GPS_TX      17     // ESP32 TX  (GPS RX)
#define METAL_PIN   32     // Metal detector (goes HIGH when metal near)
#define MOTOR_PIN   18     // MOSFET gate (vibration motor)

// -------- Globals --------
HardwareSerial SerialGPS(2);   // UART2 for GPS
TinyGPSPlus gps;
Adafruit_BMP085 bmp;

// Motor PWM (LEDC)
const int MOTOR_CH   = 0;
const int MOTOR_FREQ = 250;    // Hz (quiet, fine for ERM)
const int MOTOR_RES  = 8;      // 8-bit, duty 0..255

// Detection filter
const uint8_t SAMPLES = 20;    // take 20 quick reads
const uint8_t NEED_HIGH = 15;  // at least 15/20 HIGH → valid detect

// Cooldown so motor doesn't run nonstop
const unsigned long ALERT_PULSE_MS = 250;   // each pulse on time
const unsigned long ALERT_GAP_MS   = 150;   // gap between pulses
const uint8_t       ALERT_PULSES   = 3;     // number of pulses per event
const uint8_t       MOTOR_DUTY     = 200;   // 0..255 (strength)
const unsigned long COOLDOWN_MS    = 2500;  // ignore re-triggers for 2.5 s

unsigned long lastAlertMs = 0;

// -------- Helpers --------
bool metalTriggered() {
  // majority vote over quick samples to reject noise
  uint8_t highCount = 0;
  for (uint8_t i = 0; i < SAMPLES; i++) {
    highCount += (digitalRead(METAL_PIN) == HIGH);
    delayMicroseconds(1500); // ~1.5 ms between samples
  }
  return (highCount >= NEED_HIGH); // HIGH = metal near (your board)
}

void motorSet(uint8_t duty) {
  ledcWrite(MOTOR_CH, duty);
}

void motorPulses(uint8_t pulses, unsigned long onMs, unsigned long offMs, uint8_t duty) {
  for (uint8_t i = 0; i < pulses; i++) {
    motorSet(duty);
    delay(onMs);
    motorSet(0);
    delay(offMs);
  }
}

void printGPS() {
  if (gps.location.isValid()) {
    Serial.print("GPS Lat,Lng : ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(", ");
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println("GPS Lat,Lng : NO FIX");
  }

  if (gps.date.isValid() && gps.time.isValid()) {
    Serial.print("UTC Date/Time: ");
    Serial.print(gps.date.year()); Serial.print("-");
    Serial.print(gps.date.month()); Serial.print("-");
    Serial.print(gps.date.day()); Serial.print(" ");
    Serial.print(gps.time.hour()); Serial.print(":");
    Serial.print(gps.time.minute()); Serial.print(":");
    Serial.println(gps.time.second());
  }
}

void printBMP() {
  float temperature = bmp.readTemperature();
  float pressure    = bmp.readPressure(); // Pa
  float altitude    = bmp.readAltitude(); // uses 101325 Pa sea-level by default

  Serial.print("Altitude (m) : ");   Serial.println(altitude);
  Serial.print("Pressure (Pa): ");   Serial.println(pressure);
  Serial.print("Temp (°C)    : ");   Serial.println(temperature);
}

// -------- Setup --------
void setup() {
  Serial.begin(115200);
  Serial.println("\nLandmine Finder Shoes — starting...");

  // GPS UART
  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Serial.println("GPS UART: OK");

  // I2C + BMP180
  Wire.begin(21, 22);
  if (!bmp.begin()) {
    Serial.println("ERROR: BMP180 not detected!");
    // continue anyway, but readings will be invalid
  } else {
    Serial.println("BMP180: OK");
  }

  // IO
  pinMode(METAL_PIN, INPUT);  // you have an external pulldown (~57k)
  // Motor PWM
  ledcSetup(MOTOR_CH, MOTOR_FREQ, MOTOR_RES);
  ledcAttachPin(MOTOR_PIN, MOTOR_CH);
  motorSet(0);

  Serial.println("System ready.\n");
}

// -------- Loop --------
void loop() {
  // Feed GPS parser
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
  }

  bool detected = metalTriggered();  // HIGH = metal near

  // Report + act
  if (detected && (millis() - lastAlertMs >= COOLDOWN_MS)) {
    lastAlertMs = millis();

    Serial.println("⚠️  Landmine DETECTED (filtered HIGH) ⚠️");
    printGPS();
    printBMP();

    // haptic alert
    motorPulses(ALERT_PULSES, ALERT_PULSE_MS, ALERT_GAP_MS, MOTOR_DUTY);
  }

  // periodic telemetry (optional)
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    lastPrint = millis();
    Serial.println("---------------- Status ----------------");
    Serial.print("Metal (raw): "); Serial.println(digitalRead(METAL_PIN) ? "HIGH" : "LOW");
    printGPS();
    if (bmp.begin()) {  // only print if sensor present
      printBMP();
    }
    Serial.println("----------------------------------------\n");
  }
}
