#include <Servo.h>

Servo servoLeft;
Servo servoRight;

void setup() {
  servoLeft.attach(10);
  servoRight.attach(11);
  Serial.begin(9600);
  servoLeft.write(90);
  servoRight.write(90);
  delay(1000);
}

void loop() {
  // Test Left Servo
  Serial.println("Left: Going to 60");
  servoLeft.write(60);
  delay(1000);
  Serial.println("Left: Center");
  servoLeft.write(90);
  delay(1000);
  Serial.println("Left: Going to 120");
  servoLeft.write(120);
  delay(1000);
  Serial.println("Left: Center");
  servoLeft.write(90);
  delay(1000);

  // Test Right Servo
  Serial.println("Right: Going to 60");
  servoRight.write(60);
  delay(1000);
  Serial.println("Right: Center");
  servoRight.write(90);
  delay(1000);
  Serial.println("Right: Going to 120");
  servoRight.write(120);
  delay(1000);
  Serial.println("Right: Center");
  servoRight.write(90);
  delay(1000);

  // Test Both Together
  Serial.println("Both: Steer Left");
  servoLeft.write(60); servoRight.write(120);
  delay(1000);
  Serial.println("Both: Center");
  servoLeft.write(90); servoRight.write(90);
  delay(1000);
  Serial.println("Both: Steer Right");
  servoLeft.write(120); servoRight.write(60);
  delay(1000);
  Serial.println("Both: Center");
  servoLeft.write(90); servoRight.write(90);
  delay(1000);
}