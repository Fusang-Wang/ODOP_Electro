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

// Define some steppers and the pins the will use
#define EN        8  

//Direction pin
#define X_DIR     5 
#define Y_DIR     6
#define Z_DIR     7

//Step pin
#define X_STP     2
#define Y_STP     3 
#define Z_STP     4 


//DRV8825
int delayTime=15; //Delay between each pause (uS) in  one microstep unit the lower the smoother
//int stps=6400;// Steps to move, the bigger the smoother
int stps=96000/delayTime;// Steps to move, the bigger the smoother


void step(boolean dir, byte dirPin, byte stepperPin, int steps)
{

  digitalWrite(dirPin, dir);

  delay(100);

  for (int i = 0; i < steps; i++) {
    // one micro step unit = 1/32 step (1.8 degree)
    digitalWrite(stepperPin, HIGH);

    delayMicroseconds(delayTime); 

    digitalWrite(stepperPin, LOW);

    delayMicroseconds(delayTime); 

  }

}

void setup(){

  pinMode(X_DIR, OUTPUT); pinMode(X_STP, OUTPUT);

  pinMode(Y_DIR, OUTPUT); pinMode(Y_STP, OUTPUT);

  pinMode(Z_DIR, OUTPUT); pinMode(Z_STP, OUTPUT);

  pinMode(EN, OUTPUT);

  digitalWrite(EN, LOW);

}

void loop(){

//   step(false, X_DIR, X_STP, stps/4); //X, Clockwise
//   step(false, Y_DIR, Y_STP, stps); //Y, Clockwise
//   step(false, Z_DIR, Z_STP, stps); //Z, Clockwise

//   delay(100);

//   step(true, X_DIR, X_STP, stps); //X, Counterclockwise
//   step(true, Y_DIR, Y_STP, stps); //Y, Counterclockwise
//   step(true, Z_DIR, Z_STP, stps); //X, Counterclockwise

//   delay(100);

}