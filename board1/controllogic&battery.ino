#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <ESP32Servo.h>

#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"
#define API_KEY "Your_API_Key"
#define DATABASE_URL "Your_Database_URL"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

const int AIN1 = 25;
const int AIN2 = 33;
const int BIN1 = 27;
const int BIN2 = 14;
const int PWMA = 32;
const int PWMB = 13;
const int STBY = 26;
const int SERVO1_PIN = 22;
const int SERVO2_PIN = 23;
const int ADC_3V7 = 34; 
const int ADC_7V4 = 35; 

String listenerPath = "/board1";

int direction = 0;
const int maxSpeedA = 150; 
const int maxSpeedB = 100;
int speedA = 0;
int speedB = 0;
bool isConnected = false;
//int currentSpeed = 0;
//int speed = 0;
int servo1Position = 0;
int servo2Position = 0;

Servo servo1;
Servo servo2;

unsigned long previousMillis = 0;

void stopAllMotors() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  setMotorSpeed(0, 0);
  speedA = 0;
  speedB = 0;
  Serial.println("Motors stopped");
}

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    isConnected = true;
  } else {
    Serial.println("\nFailed to connect to WiFi");
    isConnected = false;
  }
}

void onWiFiEvent(WiFiEvent_t event) {
  switch(event) {
    case WIFI_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi disconnected, stopping motors and attempting reconnection...");
      stopAllMotors(); 
      isConnected = false;
      connectWiFi();
      break;

    case IP_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      isConnected = true;
      break;

    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.onEvent(onWiFiEvent); 
  
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(ADC_3V7, INPUT);
  pinMode(ADC_7V4, INPUT);

  if (!servo1.attached()) {
    servo1.attach(SERVO1_PIN);
  }
  if (!servo2.attached()) {
    servo2.attach(SERVO2_PIN);
  }

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  
  connectWiFi(); 

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase SignUp OK");
  } else {
    Serial.println("Firebase SignUp Failed");
  }
  
  Firebase.begin(&config, &auth); 
  Firebase.reconnectWiFi(true);   

  if (!Firebase.RTDB.beginStream(&fbdo, listenerPath.c_str())) {
    Serial.printf("Stream begin error, %s\n", fbdo.errorReason().c_str());
  }

  Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, streamTimeoutCallback);

  digitalWrite(STBY, HIGH);
}

void setMotorSpeed(int speedA, int speedB) {
  analogWrite(PWMA, speedA);
  analogWrite(PWMB, speedB);
}

void motorControl() {
  stopAllMotors(); 

  switch (direction) {
    case 0:
      decelerateMotors();
      break;
    case 1:
      Serial.println(F("Forward"));
      digitalWrite(AIN1, HIGH);
      break;
    case 2:
      Serial.println(F("Backward"));
      digitalWrite(AIN2, HIGH);
      break;
    case 3:
      Serial.println(F("Left"));
      digitalWrite(BIN1, HIGH);
      break;
    case 4:
      Serial.println(F("Right"));
      digitalWrite(BIN2, HIGH);
      break;
    case 5:
      Serial.println(F("Forward & left"));
      digitalWrite(AIN1, HIGH);
      digitalWrite(BIN1, HIGH);
      break;
    case 6:
      Serial.println(F("Forward & right"));
      digitalWrite(AIN1, HIGH);
      digitalWrite(BIN2, HIGH);
      break;
    case 7:
      Serial.println(F("Backward & left"));
      digitalWrite(AIN2, HIGH);
      digitalWrite(BIN1, HIGH);
      break;
    case 8:
      Serial.println(F("Backward & right"));
      digitalWrite(AIN2, HIGH);
      digitalWrite(BIN2, HIGH);
      break;
    default:
      Serial.println(F("Invalid direction"));
      return;
  }

  if (direction != 0) {
    accelerateMotors();
  }
}

/*void accelerateMotors() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 10) {
    previousMillis = currentMillis;
    if (currentSpeed < maxSpeed) {
      currentSpeed += 5;
      if (currentSpeed > maxSpeed) currentSpeed = maxSpeed;
      setMotorSpeed(currentSpeed);
    }
  }
}

void decelerateMotors() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 10) {
    previousMillis = currentMillis;
    if (currentSpeed > 0) {
      currentSpeed -= 10;
      if (currentSpeed < 0) currentSpeed = 0;
      setMotorSpeed(currentSpeed);
    }
  }
}*/

void decelerateMotors() {
  while (speedA > 0 || speedB > 0) {
    speedA -= 10;
    speedB -= 10;

    if (speedA < 0) speedA = 0;
    if (speedB < 0) speedB = 0;

    setMotorSpeed(speedA, speedB);
  }
}

void accelerateMotors() {
  while (speedA < maxSpeedA || speedB < maxSpeedB) {
    if (speedA < maxSpeedA) {
      speedA += 1;
      if (speedA > maxSpeedA) speedA = maxSpeedA;
    }
    if (speedB < maxSpeedB) {
      speedB += 5;
      if (speedB > maxSpeedB) speedB = maxSpeedB;
    }
    setMotorSpeed(speedA, speedB);
  }
}

void streamCallback(FirebaseStream data) {
  Serial.println(F("Stream Data Received"));
  Serial.println(data.streamPath());
  Serial.println(data.dataPath());
  Serial.println(data.dataType());
  Serial.println(data.eventType());
  Serial.print(F("Data: "));

  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
    if (data.dataPath() == "/direction") {
      direction = data.intData();
      Serial.println("Direction received: " + String(direction));
      motorControl(); 
    } else if (data.dataPath() == "/servo1") {
      servo1Position = data.intData();
      servo1.write(servo1Position);
      Serial.println("Servo 1 Position received: " + String(servo1Position));
    } else if (data.dataPath() == "/servo2") {
      servo2Position = data.intData();
      servo2.write(servo2Position);
      Serial.println("Servo 2 Position received: " + String(servo2Position));
    }
  } else {
    Serial.println("Unhandled data type or path");
  }
}

void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Firebase stream timed out, restarting stream...");
    if (!Firebase.RTDB.beginStream(&fbdo, String(listenerPath.c_str()))) {
      Serial.printf("Firebase stream restart error: %s\n", fbdo.errorReason().c_str());
    } else {
      Serial.println("Firebase stream restarted successfully.");
    }
  }
}

int getBatteryPercentage(int pin, bool is3v7) {
  float R1, R2, minV, maxV;

  if (is3v7) {
    R1 = 1000.0;  // 1KΩ
    R2 = 2200.0;  // 2.2KΩ
    minV = 3.2;   // Discharge voltage
    maxV = 4.2;   // Fully charged voltage
  } else {
    R1 = 2200.0;  // 2.2KΩ
    R2 = 1000.0;  // 1KΩ
    minV = 6.4;   // Discharge voltage
    maxV = 8.4;   // Fully charged voltage
  }

  int raw = analogRead(pin);
  float voltage = (raw / 4095.0) * 3.3 * (R1 + R2) / R2;
  
  return constrain(((voltage - minV) / (maxV - minV)) * 100, 0, 100);
}

void loop() {
  if (millis() - previousMillis >= 30000) {  
    previousMillis = millis();

    int charge3v7 = getBatteryPercentage(ADC_3V7, true);
    int charge7v4 = getBatteryPercentage(ADC_7V4, false);

    Serial.println("Battery percentages calculated:");
    Serial.println("3.7V Battery: " + String(charge3v7) + "%");
    Serial.println("7.4V Battery: " + String(charge7v4) + "%");

    Firebase.RTDB.setInt(&fbdo, "/board1/3v7", charge3v7);
    Firebase.RTDB.setInt(&fbdo, "/board1/7v4", charge7v4);

    Serial.println("Battery percentages updated to Firebase.");
  }
}
