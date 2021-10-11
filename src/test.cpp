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

#define STEPS_FULL  6400  // Number of microsteps in a full rotation

// speed
#define DELAY_TIME  1000  // Delay between each pause (uS) in  one microstep unit — the lower the smoother

float absoluteAngle = 0.;


void step (boolean dir, byte dirPin, byte stepperPin, int steps, int delayTime) {

  // Set rotation direction
  digitalWrite(dirPin, dir);

  for (int i = 0; i < steps; i++) {

    // Run one microstep unit
    digitalWrite(stepperPin, HIGH);
    delayMicroseconds(delayTime); 

    digitalWrite(stepperPin, LOW);
    delayMicroseconds(delayTime); 

  }
}


void stepAngle (boolean dir, byte dirPin, byte stepperPin, float angleDeg, int delayTime) {

  // Serial.print ("Stepping ");
  // Serial.print (angleDeg);
  // Serial.println (" deg");


  if (dir == false) {
    absoluteAngle += 1. * angleDeg;
  } else if (dir == true) {
    absoluteAngle += -1. * angleDeg;
  };

  int steps = round (angleDeg / 360 * STEPS_FULL);
  step (dir, dirPin, stepperPin, steps, delayTime);
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

  // stepAngle(false, X_DIR, X_STP, 90., 500); delay(2000); stepAngle(true, X_DIR, X_STP, 90., 500); delay(2000);


  for (int i = 0; i < 200; i ++) {
    // stepAngle (false, X_DIR, X_STP, 360, 20000);  // 10 steps
    // stepAngle (false, X_DIR, X_STP, 1.8, 500);  // full step
    // stepAngle (false, X_DIR, X_STP, 0.9, 200);  // 1/2 microstep
    // stepAngle (false, X_DIR, X_STP, 0.45, 100);  // 1/4 microstep
    // stepAngle (false, X_DIR, X_STP, 0.225, 100);  // 1/8 microstep
    // stepAngle (false, X_DIR, X_STP, 0.1125, 100);  // 1/16 microstep >> resolution = 0.1125deg
    // stepAngle (false, X_DIR, X_STP, 0.05625, 100);  // 1/32 microstep (not possible)

    Serial.print ("Angle: ");
    Serial.print (absoluteAngle);
    Serial.println (" deg");

    delay(500);
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