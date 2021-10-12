// MultiStepper.pde
// -*- mode: C++ -*-
//
// Shows how to multiple simultaneous steppers
// Runs one stepper forwards and backwards, accelerating and decelerating
// at the limits. Runs other steppers at the same time
//
// Copyright (C) 2009 Mike McCauley
// $Id: MultiStepper.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>
#include <math.h>

// Define some steppers and the pins the will use
#define EN          8  

//Direction pin
#define X_DIR       5 
#define Y_DIR       6
#define Z_DIR       7

// Step pin
#define X_STP       2
#define Y_STP       3 
#define Z_STP       4 

#define MICROSTEPS_FULL  6400  // Number of microsteps in a full rotation cste


float angleAbsolute = 0.;


void step (boolean dir, byte dirPin, byte stepperPin, int steps, int delayTime) {

  // Set rotation direction
  digitalWrite(dirPin, dir);

  for (int i = 0; i < steps; i++) {

    // Run one 1/32 microstep unit
    digitalWrite(stepperPin, HIGH);
    delayMicroseconds(delayTime); 

    digitalWrite(stepperPin, LOW);
    delayMicroseconds(delayTime); 

  }
}


float fitToSteps (float angleDeg) {

  int64_t angleInt = round (angleDeg * 100000);
  float remainder = angleInt % 5625; // minimum angle (1/32), * 100000
  float angleFit = (angleInt - remainder) / 100000;

  return angleFit;
}


float stepAngle (boolean dir, byte dirPin, byte stepperPin, float angleDeg, int delayTime) {

  // 'fit' into base 1.8
  float angleFit = fitToSteps (angleDeg);
  Serial.print("Stepping "); Serial.print(angleFit); Serial.print(" deg / "); Serial.print(angleDeg); Serial.println(" deg.");

  // Step command
  int microsteps = round (angleFit / 360 * MICROSTEPS_FULL);  // round shouldn't be needed     
  step (dir, dirPin, stepperPin, microsteps, delayTime);

  // Add sign
  if (dir == true) { angleFit *= -1.; };
  return angleFit; // return actual angle
}





void setup(){

  Serial.begin(9600);

  pinMode(X_DIR, OUTPUT); pinMode(X_STP, OUTPUT);
  pinMode(Y_DIR, OUTPUT); pinMode(Y_STP, OUTPUT);
  pinMode(Z_DIR, OUTPUT); pinMode(Z_STP, OUTPUT);

  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);

}





void loop() {


  // float angleCommand = 0.;

  for (int i = 0; i < 200; i ++) {
    // angleAbsolute += stepAngle (false, X_DIR, X_STP, 120, 100);
    // float error = angleCommand - angleAbsolute;
    // Serial.println(error);
  }


  for (int i = 0; i < 200; i ++) {
    // stepAngle (false, X_DIR, X_STP, 360, 20000);  // 10 steps
    // stepAngle (false, X_DIR, X_STP, 1.8, 500);  // full step
    // stepAngle (false, X_DIR, X_STP, 0.9, 200);  // 1/2 microstep
    // stepAngle (false, X_DIR, X_STP, 0.45, 100);  // 1/4 microstep
    // stepAngle (false, X_DIR, X_STP, 0.225, 100);  // 1/8 microstep
    // stepAngle (false, X_DIR, X_STP, 0.1125, 100);  // 1/16 microstep >> resolution = 0.1125deg
    // stepAngle (false, X_DIR, X_STP, 0.05625, 100);  // 1/32 microstep (not possible)

    // Serial.print ("Angle: ");
    // Serial.print (absoluteAngle);
    // Serial.println (" deg");

    // delay(500);
  }

  delay (3000);
  
  
  

  // step(false, X_DIR, X_STP, stps/4); //X, Clockwise
  // step(false, Y_DIR, Y_STP, stps); //Y, Clockwise
  // step(false, Z_DIR, Z_STP, stps); //Z, Clockwise

  // delay(100);

  // step(true, X_DIR, X_STP, stps*4); //X, Counterclockwise
  // step(true, Y_DIR, Y_STP, stps); //Y, Counterclockwise
  // step(true, Z_DIR, Z_STP, stps); //X, Counterclockwise

  // delay(100);
  

}