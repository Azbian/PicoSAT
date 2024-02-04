#include <Stepper.h>

// Define the number of steps per revolution for each motor
const int stepsPerRevolution = 200;

// Define the pins connected to the L298N motor driver
const int enAPin = 9;   // Enable pin for motor Azimuthal
const int in1Pin = 8;   // Input pin 1 for motor Azimuthal
const int in2Pin = 7;   // Input pin 2 for motor Azimuthal
const int enEPin = 6;   // Enable pin for motor Elivation
const int in3Pin = 5;   // Input pin 1 for motor Elivation
const int in4Pin = 4;   // Input pin 2 for motor Elivation

// Define the number of steps to move for each motor
int stepsAz = 0;
int stepsEl = 0;
int flag = 0;

float angleAz = 0;
float angleEl = 0;

// Create instances of the Stepper class for each motor
Stepper motorAz(stepsPerRevolution, in1Pin, in2Pin);
Stepper motorEl(stepsPerRevolution, in3Pin, in4Pin);

void setup() {
  // Set the enable pins and input pins as output pins
  pinMode(enAPin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enEPin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);

  // Set the speed of each motor
  motorAz.setSpeed(100);
  motorEl.setSpeed(100);
  Serial.setTimeout(1);
}

void loop() {
  // Read the input from the Serial monitor
  if (Serial.available() > 0) {
    // Read the input as two integers separated by a comma
    String input = Serial.readStringUntil('\n');
    int commaIndex = input.indexOf(',');
    float tempAz=0,tempEl=0;
    tempAz = input.substring(0, commaIndex).toFloat();
    tempEl = input.substring(commaIndex + 1).toFloat();

    // Calculate the number of steps to move for each motor
    stepsAz = ((tempAz - angleAz) * stepsPerRevolution) / 360;
    stepsEl = ((tempEl - angleEl) * stepsPerRevolution) / 360;

    // Enable the motors
    digitalWrite(enAPin, HIGH);
    digitalWrite(enEPin, HIGH);

    // Move each motor the desired number of steps
    if (stepsAz > 0) {
      motorAz.step(stepsAz);
    }
    if (stepsEl > 0) {
      motorEl.step(stepsEl);
    }

    // Disable the motors


    digitalWrite(enAPin, LOW);
    digitalWrite(enEPin, LOW);
    angleAz = tempAz;
    angleEl = tempEl;
    if (angleEl < 0) {
      flag = 1;
    }
  }
  if (flag && angleEl==0) {
    motorAz.step(-((angleAz * stepsPerRevolution) / 360));
    motorEl.step(-((angleEl * stepsPerRevolution) / 360));
    angleAz = 0;
    angleEl = 0;
  }
}
