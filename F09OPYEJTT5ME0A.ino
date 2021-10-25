/*  05/25/2019, by rickbme - DFS
 * 
 * A simple stepper motor tester. My coding is far from efficient, so go easy on my noobish scribble. :)
 * What started out as some short test code got a little longer as I wanted to see more info. ahh the search for knowledge.
 * 
 * All the information I found searching for steppers and microstepping was either too basic, or went into so
 * much detail my eyes glossed over after a few pages. This is the result of my own take on steppers and microstepping.
 * My enjoyment comes from finding out how things work more than actually using them to build something :) ya I know, Weird!
 * Anyway,  here's a little something I came up with to satisfy my curiosity about steppers and microstepping.
 * It's a bit over commented, but I wanted to try and answer any questions that may arise while looking it over.
 * 
 * 
 * Code was written for an Arduino Nano, a DRV8825 stepper driver board and a Nema17 stepper motor (17HS4401S).
 * Also compiled and tested on an UNO R3 and a MEGA2650 R3
 * 
 * 
 * Hope this helps someone in a project or maybe just wants to know how your 3D printer or maybe CNC actually does those really smooth moves.
 * Free to use in anyway you like.
 */

#include <AccelStepper.h>         // This is one nice library for steppers. Kudos to the author Mike McCauley @ airspayce.com
                                  // You'll find the library here https://github.com/ericbarch/arduino-libraries/tree/master/AccelStepper
                                  // Or you can also install it in the arduino library manager.


//  Define a stepper and the pins it will use
//  stepper(driver, 3(step), 2(dir))
//  AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
//  Initializes the stepper control library and defines initial values

//  This part was a bit confusing but after locating the <AccelStepper.h> manual it made sense.
//  If your using a driver board/chip such as what we're using here (the DRV8825),  we define the first parameter as "DRIVER"
//  meaning we're using a driverboard with 2 pins to control our 4 pin stepper motor. The next 2 parameters are simply the
//  arduino pins connected to the step pin and dir pin on the driver board. You could alternatly just use integer values
//  instead of the enum, but it was preferred this way. :)

//  One more tidbit juuuust in case. Don't try and drive this stepper motor with just an Arduino.  Magic smoke will escape!
//  You'll need something with a bit more power handling.  Hey,  Like this stepper driver board we're using!

//  DRIVER            Stepper Driver, 2 driver pins required. 
//  FULL2WIRE         2 wire stepper, 2 motor pins required 
//  FULL3WIRE         3 wire stepper, such as HDD spindle, 3 motor pins required 
//  FULL4WIRE         4 wire full stepper, 4 motor pins required
//  HALF3WIRE         3 wire half stepper, such as HDD spindle, 3 motor pins required 
//  HALF4WIRE         4 wire half stepper, 4 motor pins required 
// 

enum
stepper_Motor {
  FUNCTION = 0, DRIVER = 1, FULL2WIRE = 2, FULL3WIRE = 3,
  FULL4WIRE = 4, HALF3WIRE = 6, HALF4WIRE = 8
};

// Define pins on the Arduino for microstepping control on DRV8825 ( M0, M1, M2) MODE0, MODE1, MODE2 on the IC for you datasheet gurus ;)
#define M_ZERO 4
#define M_ONE 5
#define M_TWO 6
#define sleep_pwr 7

//uint32_t T1, T2, dT;
unsigned long start_Time, end_Time, delta_Time;

AccelStepper stepper_Motor(DRIVER, 3, 2); //stepper(driver, step, dir)

void setup() {
Serial.begin(9600);

pinMode(M_ZERO, OUTPUT);
pinMode(M_ONE, OUTPUT);
pinMode(M_TWO, OUTPUT);
pinMode(sleep_pwr, OUTPUT);


// Here's the pin settings on the DRV8825 that determines the steps and how many steps is in one revolution

//         M0   M1    M2          RESOLUTION
//------------------------------------------------
//        LOW   LOW   LOW        FULL  STEP   1 Revolution  200  steps
//        HIGH  LOW   LOW        HALF  STEP   1 Revolution  400  steps
//        LOW   HIGH  LOW        1/4   STEP   1 Rrevolution  800  steps
//        HIGH  HIGH  LOW        1/8   STEP   1 Rrevolution  1600 steps
//        LOW   LOW   HIGH       1/16  STEP   1 Revolution  3200 steps
//        HIGH  LOW   HIGH       1/32  STEP   1 Revolution  6400 steps  1500

digitalWrite(M_ZERO,LOW);                         // Lets just start with a base line Full Step
digitalWrite(M_ONE,LOW);
digitalWrite(M_TWO,LOW);
digitalWrite(sleep_pwr,LOW);

stepper_Motor.setCurrentPosition(0);

}

void loop() {

  if(stepper_Motor.distanceToGo()==0)  {

    float stepper_time;
    end_Time = micros();
    delta_Time = end_Time - start_Time;
    stepper_time = delta_Time /1000000.0,2;
    Serial.println((String)"Time taken " + (stepper_time)+ " seconds");           // Don't know how accurate the time elapse is. I just wanted a quick idea of time.
    Serial.println();
    digitalWrite(sleep_pwr,LOW);                                                  // Let's turn off the stepper so it doesn't get hot sitting there while we play!
    delay(100);
    Serial.println("Let's get to work!  How many revolutions should we do?");     //  Don't be like me and hit 9 on a 1/32 step!
    while (Serial.available()==0);                                                // You can, however, enter something really small like .01
    float rev=Serial.parseFloat();
    Serial.print(rev);Serial.println(" revolution(s)");

    Serial.println("What step would you like to test?");
    Serial.println("Options");
    Serial.println("1. Full Step");
    Serial.println("2. Half Step");
    Serial.println("3. 1/4 Step");
    Serial.println("4. 1/8 Step");
    Serial.println("5. 1/16 Step");
    Serial.println("6. 1/32 Step");
  
    while (Serial.available()==0);
    float step_Option=Serial.parseFloat();
    if(step_Option == 1){                                                          // 200 appears to be about the maximum reliable speed of a 17HS4401S at full step. 
      Serial.println("Using Full Step");                                           //Increasing current had little effect. Lowering the accelleration helped some.
      Serial.println("What speed would you like? Try 100 at first");              // This was at .87 vref. This is about 830ma. Power supply was set at 12.00v
      Serial.println("Options are 0-10000");                                      // This was measured at the base of the potentiometer using logic ground.
      while (Serial.available()==0);                                              // Motor ground will give a different reading.I noticed that by using the logic
                                                                                   // ground the vref just about indicates current draw. IE, .87v = .830a
      float stepper_Speed=Serial.parseFloat();
      if (stepper_Speed < 10001){
        Serial.println((String)"Using " + (stepper_Speed) + " Speed");
        stepper_Motor.setMaxSpeed(stepper_Speed);          // I noticed at lower vref settings and hence, lower amperage draw, that we see steps getting
                                                           // skipped and increasingly more erratic operation as we increase speed which makes sense.
        Serial.println("What acceleration would you like?  Try 200 at first");
        Serial.println("Options are 0-5000");               // These "Suggestions" I've added are reliable numbers to get a feel for things.
        while (Serial.available()==0);                      // Afterwards you can get all crazy if you want. Just don't blow up your stepper and driver!
        float stepper_Accell=Serial.parseFloat();           // I got mine pretty hot trying different extremes but never blew anything. :)
                                                            // Don't blame me if you try for 1,000 rpms and the magic smoke escapes!.
        if (stepper_Accell < 5001){
          stepper_Motor.setAcceleration(stepper_Accell);
          Serial.println((String)"Using " + (stepper_Accell) + " Acceleration");

          unsigned long int motor_Steps=rev*200;          // From what I have read anything over 1000 steps are unreliable. So consider that while
          Serial.print(motor_Steps);                      // Playing with the numbers
          Serial.println((String)" motor_Steps @ " +(stepper_Speed)+ " speed");

          digitalWrite(M_ZERO,LOW);                       // Here's where the magic happens. This combination gives us full steps.
          digitalWrite(M_ONE,LOW);                        // Our stepper is rated at 1.8deg per step. A FULL step. So 360Deg/1.8Deg = 200 steps
          digitalWrite(M_TWO,LOW);                        // per revolution. Each time we half the step, we double the steps per revolution.
          digitalWrite(sleep_pwr,HIGH);                   // Which in turn increases our potential accuracy and our noise is reduced as well. Yay!
          stepper_Motor.setCurrentPosition(0);

         start_Time = micros();
          stepper_Motor.moveTo(motor_Steps);

         } else {
            error_handler();                               // up the values to bypass this annoying lockout!
         }
      } else {
      error_handler();
      }

    }
    if(step_Option == 2){                                 // Surprisingly this seems to be the best for general use.
      Serial.println("Using Half Step");
      Serial.println("What speed would you like? Try 300 at first");
      Serial.println("Options are 0-10000");
      while (Serial.available()==0);
      float stepper_Speed=Serial.parseFloat();

      if (stepper_Speed < 100001){
        Serial.println((String)"Using " + (stepper_Speed) + " Speed");
        stepper_Motor.setMaxSpeed(stepper_Speed);

        Serial.println("What acceleration would you like? Try 300 at first");
        Serial.println("Options are 0-5000");
        while (Serial.available()==0);
        float stepper_Accell=Serial.parseFloat();
        if (stepper_Accell < 5001){
          stepper_Motor.setAcceleration(stepper_Accell);
          Serial.println((String)"Using " + (stepper_Accell) + " Acceleration");

          unsigned long int motor_Steps=rev*400;
          Serial.print(motor_Steps);
          Serial.println((String)" motor_Steps @ " +(stepper_Speed)+ " speed");
          digitalWrite(M_ZERO,HIGH);                    // With this combination we're half stepping, and doubling the amount of
          digitalWrite(M_ONE,LOW);                      // steps it takes for one complete revolution. We're into Microstepping now! woooooo!
          digitalWrite(M_TWO,LOW);
          digitalWrite(sleep_pwr,HIGH);
          stepper_Motor.setCurrentPosition(0);
          start_Time = micros();
          stepper_Motor.moveTo(motor_Steps);

          } else {
          error_handler();
          
      }
      }else {
        error_handler();
      }
    }
    if(step_Option == 3){
      Serial.println("Using 1/4 Step");
      Serial.println("What speed would you like? Try 600 at first");
      Serial.println("Options are 0-10000");        // We can bump the speed up now
      while (Serial.available()==0);
      float stepper_Speed=Serial.parseFloat();
      if (stepper_Speed < 10001){
        Serial.println((String)"Using " + (stepper_Speed) + " Speed");
        stepper_Motor.setMaxSpeed(stepper_Speed);

        Serial.println("What acceleration would you like? Try 500 at first");
        Serial.println("Options are 0-5000");
        while (Serial.available()==0);
        float stepper_Accell=Serial.parseFloat();
        if (stepper_Accell < 5001){
          stepper_Motor.setAcceleration(stepper_Accell);
          Serial.println((String)"Using " + (stepper_Accell) + " Acceleration");

          unsigned long int motor_Steps=rev*800;
          Serial.print(motor_Steps);
          Serial.println((String)" motor_Steps @ " +(stepper_Speed)+ " speed");
          digitalWrite(M_ZERO,LOW);
          digitalWrite(M_ONE,HIGH);
          digitalWrite(M_TWO,LOW);
          digitalWrite(sleep_pwr,HIGH);
          stepper_Motor.setCurrentPosition(0);
          start_Time = micros();
          stepper_Motor.moveTo(motor_Steps);

          } else {
          error_handler();

      }
      }else {
        error_handler();
      }
    }

    if(step_Option == 4){                                               // This step and higher we run into issues with Back EMF limiting our speed
      Serial.println("Using 1/8 Step");                                 // without getting more complex. Ie more voltage, different driver ic's, and more.
      Serial.println("What speed would you like? Try 800 at first");   // Even with these limiting factors it's fun to see what it can do.
      Serial.println("Options are 0-10000");
      while (Serial.available()==0);
      float stepper_Speed=Serial.parseFloat();
      if (stepper_Speed < 10001){
        Serial.println((String)"Using " + (stepper_Speed) + " Speed");
        stepper_Motor.setMaxSpeed(stepper_Speed);

        Serial.println("What acceleration would you like? Try 600 at first");
        Serial.println("Options are 0-5000");
        while (Serial.available()==0);
        float stepper_Accell=Serial.parseFloat();
        if (stepper_Accell < 5001){
          stepper_Motor.setAcceleration(stepper_Accell);
          Serial.println((String)"Using " + (stepper_Accell) + " Acceleration");

          unsigned long int motor_Steps=rev*1600;
          Serial.print(motor_Steps);
          Serial.println((String)" motor_Steps @ " +(stepper_Speed)+ " speed");
          digitalWrite(M_ZERO,HIGH);
          digitalWrite(M_ONE,HIGH);
          digitalWrite(M_TWO,LOW);
          digitalWrite(sleep_pwr,HIGH);
          stepper_Motor.setCurrentPosition(0);
          start_Time = micros();
          stepper_Motor.moveTo(motor_Steps);

          } else {
          error_handler();

      }
      }else {
        error_handler();
      }
    }

    if(step_Option == 5){                                       // 
      Serial.println("Using 1/16 Step");
      Serial.println("What speed would you like? Try 1000 at first");
      Serial.println("Options are 0-10000");
      while (Serial.available()==0);
      float stepper_Speed=Serial.parseFloat();
      if (stepper_Speed < 10001){
        Serial.println((String)"Using " + (stepper_Speed) + " Speed");
        stepper_Motor.setMaxSpeed(stepper_Speed);                      // And even higher. If you select like "10", prepare for a wait!

        Serial.println("What acceleration would you like? Try 1000 at first");
        Serial.println("Options are 0-5000");
        while (Serial.available()==0);
        float stepper_Accell=Serial.parseFloat();
        if (stepper_Accell < 5001){
          stepper_Motor.setAcceleration(stepper_Accell);
          Serial.println((String)"Using " + (stepper_Accell) + " Acceleration");

          unsigned long int motor_Steps=rev*3200;
          Serial.print(motor_Steps);
          Serial.println((String)" motor_Steps @ " +(stepper_Speed)+ " speed");
          digitalWrite(M_ZERO,LOW);
          digitalWrite(M_ONE,LOW);
          digitalWrite(M_TWO,HIGH);
          digitalWrite(sleep_pwr,HIGH);
          stepper_Motor.setCurrentPosition(0);
          start_Time = micros();
          stepper_Motor.moveTo(motor_Steps);

          } else {
          error_handler();
          }
      }else {
        error_handler();
      }
    }

    if(step_Option == 6){                                     // Great smooth action and control,  but our top speed is now an issue.
      Serial.println("Using 1/32 Step");
      Serial.println("What speed would you like? Try 2000 at first");
      Serial.println("Options are 0-10000");
      while (Serial.available()==0);
      float stepper_Speed=Serial.parseFloat();

      if (stepper_Speed < 10001){
        Serial.println((String)"Using " + (stepper_Speed) + " Speed");
        stepper_Motor.setMaxSpeed(stepper_Speed);             // And even higher. If you select like "25", go play some monopoly.

        Serial.println("What acceleration would you like? Try 2000 at first");
        Serial.println("Options are 0-5000");
        while (Serial.available()==0);
        float stepper_Accell=Serial.parseFloat();
        if (stepper_Accell < 5001){
          stepper_Motor.setAcceleration(stepper_Accell);
          Serial.println((String)"Using " + (stepper_Accell) + " Acceleration");

          unsigned long int motor_Steps=rev*6400;
          Serial.print(motor_Steps);
          Serial.println((String)" motor_Steps @ " +(stepper_Speed)+ " speed");
          digitalWrite(M_ZERO,HIGH);
          digitalWrite(M_ONE,LOW);
          digitalWrite(M_TWO,HIGH);
          digitalWrite(sleep_pwr,HIGH);
          stepper_Motor.setCurrentPosition(0);
          start_Time = micros();
          stepper_Motor.moveTo(motor_Steps);

          } else {
          error_handler();
          }
      } else {
        error_handler();
      }
    }

    if(step_Option!= 1 && step_Option!= 2 && step_Option!= 3 && step_Option!= 4 && step_Option!= 5 && step_Option!= 6) {
      error_handler();
    }

  }

  stepper_Motor.run();                      // Here's where we "step" through to the desired number of steps

}

void error_handler(){
Serial.println("Not a valid option, please try again");
}
  //  I hope this helped you in someway better undertand steppers and microstepping. I know I learned alot.
  //  Now don't be like me! Go build something with your steppers!!
  //  I have to go figure something else out! :D

  //  Rick
