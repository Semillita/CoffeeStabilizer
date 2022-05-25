//:: LIBRARIES :://
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include <Servo.h>

//:: VARIABLES & KONSTANTER :://
struct FrameMovement {
  float angleX;
  float angleY;
};

struct RollPitch { //Användning av structs för att smidigt packetera ihop X och Y koordinater på gyron
  float roll;
  float pitch;
};

float rollAve;
float pitchAve;

MPU6050 accelgyro;
Servo servo1;
Servo servo2;

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;

float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;

float lastTime;

float servoAngleX;
float servoAngleY;

RollPitch samples[10];

//:: FUNKTIONER :://
void setup() //Etablerar kopplingar mellan servos, gyro och arduino
{
  Wire.begin();
  Serial.begin(9600);
  servo1.attach(9);
  servo2.attach(11);

  accelgyro.initialize();
}

//Konstant loop som säger åt servomotorerna att röra sig till en viss position
void loop() {
  while (true) {
    doServoWork(getFrameMovement(0));
    delay(5); //Vi vill inte uppdatera servomotorerna alltför mycket
  }
}

//Funktion som returnerar vilken rörelse/vilken position servomotorerna ska röra sig till
FrameMovement getFrameMovement(float deltaTime) {

  float rollSum = 0;
  float pitchSum = 0;

  //För att platta till rörelsen tar vi 10 samples och räknar ut medelvärdet på dem
  const int sampleAmount = 10; 
  for (int i = 0; i < sampleAmount; i++) {
    RollPitch sample = getRollPitch();
    rollSum += sample.roll;
    pitchSum += sample.pitch;
    delay(1);
  }
  rollAve = rollSum / sampleAmount;
  pitchAve = pitchSum / sampleAmount;

  FrameMovement movement;
  movement.angleX = pitchAve;
  movement.angleY = rollAve;
  
  return movement; //I form av struct
}

//Funktion som säger till servomotorerna att flytta till en viss position (Tar in struct)
void doServoWork(FrameMovement movement) {
  float angleX = movement.angleX;
  float angleY = movement.angleY;

  servoAngleX = angleX;
  servoAngleY = angleY;

  Serial.print(servoAngleX);
  Serial.print(" ");
  Serial.println(servoAngleY);
  servo1.write(servoAngleX);
  servo2.write(servoAngleY);
}

float getDeltaTime() {
  float currentTime = millis() * 1000;
  float deltaTime = currentTime - lastTime;
  lastTime = currentTime;
  return deltaTime;
}

RollPitch getRollPitch() {
  accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
  accelgyro.getRotation(&gx, &gy, &gz);

  AccX = ax / 16384.0;
  AccY = ay / 16384.0;
  AccZ = az / 16384.0;

  accAngleY = (atan2(AccY, AccZ)+PI) * 180.0 / PI;
  accAngleX = (atan2(-AccX, AccZ)+PI) * 180.0 / PI;

  GyroX = gx / 131.0;
  GyroY = gy / 131.0;
  GyroZ = gz / 131.0;  

  GyroX += 140;
  GyroY += -20;
  GyroZ += 0;

  roll = 0.99 * GyroY + 0.01 * accAngleY;
  pitch = 0.99 * GyroX + 0.01 * accAngleX;

  RollPitch rollPitch;
  rollPitch.roll = roll;
  rollPitch.pitch = pitch;

  return rollPitch;
}
