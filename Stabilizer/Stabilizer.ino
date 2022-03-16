#include <Servo.h>
 
Servo servo1;
Servo servo2;

int angleX;
int angleY;

float lastTime;

struct FrameMovement {
  float angleX;
  float angleY;
};
 
void setup()
{
  Serial.begin(9600);
  servo1.attach(9);
  servo2.attach(11);
}
 
void loop()
{
  float deltaTime = getDeltaTime();

  FrameMovement movement = getFrameMovement(deltaTime);
  
  doServoWork(movement);
}

FrameMovement getFrameMovement(float deltaTime) {
  FrameMovement movement;
  movement.angleX = 0.0f;
  movement.angleY = 0.0f;
  
  return movement;
}

void doServoWork(FrameMovement movement) {
  
}

float getDeltaTime() {
  float currentTime = millis() * 1000;
  float deltaTime = currentTime - lastTime;
  lastTime = currentTime;
  return deltaTime;
}
