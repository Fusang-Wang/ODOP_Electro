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
//#include <Utility.h>


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

// End stops (https://forum.arduino.cc/t/using-end-stops-on-cnc-shield-design-for-gbrl/510231/2)
#define X_LIM       9
#define Y_LIM       10
#define Z_LIM       11

#define MICROSTEPS_FULL  6400  // Number of microsteps in a full rotation
#define ANGLE_MIN   -15.
#define ANGLE_MAX   90.

float angleAbsolute = 0.;
int x_lim = 0;


float baseAngle = 0.45; // Smallest angle for decomposition of large angular commands


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

int sign(float number){
  if (number > 0){
    return 1;
  }
  else {
    return -1;
  }
}


float fitToSteps (float angleDeg) {

  int64_t angleInt = round (angleDeg * 100000);
  float remainder = angleInt % 5625; // minimum angle (1/32), * 100000

  float angleFit = (angleInt - remainder) / 100000;
  return angleFit;
}


float stepAngle (byte dirPin, byte stepperPin, float angleDeg, int delayTime) {

  // Unpack angleDeg
  boolean dir;
  int direction = sign(angleDeg);
  if (direction >= 0) { dir = true; }
  else { dir = false; }
  angleDeg = abs(angleDeg);

  // 'fit' into base 1.8
  float angleFit = fitToSteps (angleDeg);
  Serial.print("Stepping "); Serial.print(angleFit, 6); Serial.print(" deg / "); Serial.print(angleDeg, 6); Serial.println(" deg.");

  // Step command
  int microsteps = round (angleFit / 360 * MICROSTEPS_FULL);  // round shouldn't be needed
  step (dir, dirPin, stepperPin, microsteps, delayTime);

  // Add sign
  if (dir == true) { angleFit *= -1.; };
  return angleFit; // return actual angle
}

/**
 * Check whether end stop is reached and return angleAbsolute value (given context)
 */

boolean checkEndStops () {
  x_lim = digitalRead (X_LIM);
  if (x_lim == 0) { return true; }
  else { return false; }
}

float updatePosition () {
  x_lim = digitalRead (X_LIM);

  if (checkEndStops()) {
    if ((angleAbsolute - ANGLE_MIN) > (ANGLE_MAX - angleAbsolute)) {
      return ANGLE_MAX; // +90 end stop
    } else {
      return ANGLE_MIN; // -15 end stop
    }
  } else {
    return angleAbsolute;
  }
}


void setup() {

  Serial.begin(9600);

  pinMode(X_DIR, OUTPUT); pinMode(X_STP, OUTPUT); pinMode(X_LIM, INPUT);
  pinMode(Y_DIR, OUTPUT); pinMode(Y_STP, OUTPUT); pinMode(Y_LIM, INPUT);
  pinMode(Z_DIR, OUTPUT); pinMode(Z_STP, OUTPUT); pinMode(Z_LIM, INPUT);

  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);

}





void loop() {

  // Hard-coded command (will be received from SERIAL)
  String command = "calibrate"; // calibrate, step, …
  
  Serial.println(); Serial.println("========== NEW LOOP ==========");

  x_lim = digitalRead (X_LIM); // needed?
  // Serial.print("X_LIM: "); Serial.println(x_lim); // Endstop switch (0 when depressed, for both X- and X+; 1 when pressed)
  
  Serial.println(100 / baseAngle);

// ###############################################################################
  // Calibration
  if (command.startsWith("calibrate")) {

    // Find motion range start (the while loop terminates)
    int iter = 0;
    while (x_lim != 0) {

      x_lim = digitalRead (X_LIM);
      angleAbsolute += stepAngle (Y_DIR, Y_STP, 1.8, 100); // 44RPM

      if (iter > 100) { break; }
      iter ++;
    }

    if (iter <= 100) {
      Serial.println("motion range start found");
      angleAbsolute = -15.; // Reset absolute angle
    } else {
      Serial.println("ERROR: exceeded iterations");
    }

  }

// ############################################################

  // Stepping (obsolete)
  if (command.startsWith("step")) {
    angleAbsolute += stepAngle (X_DIR, X_STP, 0.45, 100); // 44RPM
  }

  // Angle
  if (command.startsWith("angle 1.12")) {

    // Extract angular command
    float angleCommand = command.substring(5).toFloat();

    // Calculate target angle after task completion
    float angleTarget = angleAbsolute + angleCommand;

    while (true) { // DEFINE LOOP

      if (checkEndStops()) {
        angleAbsolute = updatePosition();
        break; // return error message
      } else {
        angleAbsolute += stepAngle (X_DIR, X_STP, baseAngle, 100); // 44RPM
      }
      
      //float error = abs(angleTarget - angleAbsolute);
      // QUANTIFY ANGLE LEFT TO MOVE FOR
    }

    // return confirmation
  }


  delay(3000);

  for (int i = 0; i < 200; i ++) {
    // angleAbsolute += stepAngle (X_DIR, X_STP, 360, 100); // 44RPM
    // float error = angleCommand - angleAbsolute;
    // Serial.println(error);
    // delay(100);
  }

}


// Stepping 119.981246 deg / 120.000000 deg.

