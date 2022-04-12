
#include <Servo.h>
#include <Arduino.h>
#include <OneButton.h>

int hallSensor = 7;
unsigned long hallTime; // calc delay between hall sensor interrupt
unsigned long nextTime;
int magnetsCount = 12;
float rpm_;
float rpm;

float rpm_sum_12 = 0;
float rpm_avg_12 = 0;
float rpm_avg_influence_12 = 0.2;
int count_12 = 0;

float rpm_sum_6 = 0;
float rpm_avg_6 = 0;
float rpm_avg_influence_6 = 0.2;
int count_6 = 0;

unsigned long rpm_sum_150 = 0;
int count_150 = 0;
int val;

int rpm_max = 155;

int stopButtonPin = 3;
boolean buttonCheck = false;
boolean stopButtonPress = false;

int outputPin = 15;

boolean launch = false;

Servo esc;
void rpmCalc();

OneButton stopButtonObj = OneButton(stopButtonPin, true, true);



void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(hallSensor, INPUT);


  attachInterrupt(4, rpmCalc, RISING);
  //attachInterrupt(0, stopButton, FALLING);
  esc.attach(outputPin);
  esc.writeMicroseconds(1500);
  hallTime = millis();
  stopButtonObj.attachDuringLongPress(stopButton);
  stopButtonObj.attachClick(stopButton);
  stopButtonObj.attachDoubleClick(launchF);
  stopButtonObj.setClickTicks(500);
  stopButtonObj.setPressTicks(300);

}


void output(int val) {
  if (val > 255) {
    val = 255;
  }
  esc.writeMicroseconds(map(val, 0 , 255, 1000, 2000));
  //Serial.println(val);
  //Serial.println(val);
}

void stopButton() {
  if (!buttonCheck) {
    buttonCheck = true;
  } else {
    stopButtonPress = true;
    launch = false;
    rpm = 0;
    output(0);
  }
//  delay(5);
//  if (digitalRead(stopButtonPin) == 0) {
//    stopButton();
//  }
}

void launchF() {
  if (rpm == 0) {
    launch = true;
    output(0);
    delay(500);
    output(0);
  }
}

void rpmCalc() {
  //Serial.println("kek");
  count_12++;
  count_6++;
  nextTime = millis();
  rpm_ = (nextTime-hallTime);
  rpm = 60/(rpm_*12/1000);
  rpm_sum_12 = rpm_sum_12 + rpm_;
  rpm_sum_6 = rpm_sum_6 + rpm_;
  if (count_12 == 12 ){
    rpm_avg_12 = 60/(rpm_sum_12/1000);
    count_12 = 0;
    rpm_sum_12 = 0;
  }
   if (count_6 == 6 ){
    rpm_avg_6 = 60/(rpm_sum_6*2/1000);
    count_6 = 0;
    rpm_sum_6 = 0;
  }
  if (rpm_avg_12 != 0) {
     rpm = rpm * (1.0 - rpm_avg_influence_12) + rpm_avg_12*rpm_avg_influence_12;
  }
    if (rpm_avg_6 != 0) {
     rpm = rpm * (1.0 - rpm_avg_influence_6) + rpm_avg_6*rpm_avg_influence_6;
  }
  hallTime= nextTime;

  if (rpm > rpm_max) {
    rpm = rpm_max;
  }
}

// the loop routine runs over and over again forever:
void loop() {
  stopButtonObj.tick();
  if (buttonCheck) {
      count_150++;
      rpm_sum_150 = rpm_sum_150 + (int(rpm));
      if (count_150 == 150){
        if ((rpm_sum_150/150) == int(rpm)){
          rpm = 0;
          rpm_avg_6 = 0;
          rpm_avg_12 = 0;
        }
        rpm_sum_150 = 0;
        count_150 = 0;
      }
      if (launch) {
        int rpm_is;
        if (rpm >= 8) {
          rpm_is = 1;
        } else {
          rpm_is = 0;
        }
          
        output(int(rpm) + 100 + (rpm_is*100));   
      } else if (!stopButtonPress){
        output(int(rpm) + 100);        
      }

      // print out the state of the button:
      //Serial.println(rpm);
      //val = digitalRead(hallSensor);
      //Serial.println(rpm);
      //delayMicroseconds(250);        // delay in between reads for stability
      delay(8);
      stopButtonPress = false;
  }
  //if (digitalRead(stopButtonPin) == 0) {
  //  stopButton();
  //}

}
