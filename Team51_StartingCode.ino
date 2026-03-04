#include <Wire.h>
#include "DFRobot_TCS34725.h"
#include "LiquidCrystal.h"
//#include
const int syringeMotorCyanPin = 2;
const int syringeMotorMagentaPin = 3;
const int syringeMotorYellowPin = 4;
const int syringeMotorKeyPin = 5;
const int mixerMotorPin = 6;
const int buttonPin0 = A0;
const int buttonPin1 = A1;
const int buttonPin2 = A2;
const int buttonPin3 = A3;
const int colorSensorPin0 = A4;
const int colorSensorPin1 = A5;
// Changes pins later
LiquidCrystal lcd(13, 12, 11, 10 ,9 ,8);

DFRobot_TCS34725 tcs = DFRobot_TCS34725(&Wire, TCS34725_ADDRESS,TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int C, M, Y, K, R, G, B;
char hex[6];
void setup() {
// Creates variables for storing color

lcd.begin(16,2);

lcd.clear();
lcd.setCursor(0, 0);

Serial.begin(115200);
Serial.println("Color View Test!");

if (tcs.begin()) {
  Serial.println("Found sensor");
  } 
else {
  Serial.println("No TCS34725 found ... check your connections");
  while (1); // halt!
  }

}

void loop() {
  int currScreen = 0;
  /* This is for testing variables
  Serial.print("C");
  Serial.println(C); // Prints the sensor reading and a newline
  delay(1000); // Wait for a second
  C = C + 35;
*/
switch(currScreen) {
    case 0:
      // HOME SCREEN
 
      break;
   
    case 1:
      // RGB SCANNING SCREEN
 
      break;
 
    case 2:
      // HEXADECIMAL LOOKUP SCREEN
 
      break;
 
    case 3:
      // HEXADECIMAL ENTRY SCREEN
 
      break;
 
    case 4:
      // MIXING SETUP SCREEN
 
      break;
 
    case 5:
      // MIXING PROCESS SCREEN
     
      dispense();
      break;
 
    case 6:
      // SETTINGS SCREEN
 
      break;
 
  }
}
void ColorSensor() {
  uint16_t clear, red, green, blue;
tcs.getRGBC(&red, &green, &blue, &clear);
tcs.lock();  // turn off LED

Serial.print("C:\t"); Serial.print(clear);
Serial.print("\tR:\t"); Serial.print(red);
Serial.print("\tG:\t"); Serial.print(green);
Serial.print("\tB:\t"); Serial.print(blue);
Serial.println("\t");
}

void rgbTo255(int &clear,int &red,int &green,int &blue) {
  uint32_t sum = clear;
  float R, G, B;
  R = red; R /= sum;
  G = green; G /= sum;
  B = blue; B /= sum;
  R *= 256; G *= 256; B *= 256;
  Serial.print("\t");
  Serial.print((int)R, HEX); Serial.print((int)G, HEX); Serial.print((int)B, HEX);
  Serial.println();
}

void rgbToCMYK() {
K = 1 - max(max(R, G), max(G, B));
C = (1-R-K)/(1-K);
M = (1-G-K)/(1-K);
Y = (1-B-K)/(1-K);
}

void dispense() {
// Turns motors on for amount depending on the amount of CMYK in the inteded color mix
  digitalWrite(syringeMotorCyanPin, HIGH);
  delay(getTimeC());
  digitalWrite(syringeMotorCyanPin, LOW);

  digitalWrite(syringeMotorMagentaPin, HIGH);
  delay(getTimeM());
  digitalWrite(syringeMotorMagentaPin, LOW);

  digitalWrite(syringeMotorYellowPin, HIGH);
  delay(getTimeY());
  digitalWrite(syringeMotorYellowPin, LOW);

  digitalWrite(syringeMotorKeyPin, HIGH);
  delay(getTimeK());
  digitalWrite(syringeMotorKeyPin, LOW);
}

int getTimeC() {
// Calculate time to enable Cyan motor to dispense an amount of paint based on CMYK 
}
int getTimeM() {
// Calculate time to enable Magenta motor to dispense an amount of paint based on CMYK 
}
int getTimeY() {
// Calculate time to enable Yellow motor to dispense an amount of paint based on CMYK 
}
int getTimeK() {
// Calculate time to enable Key motor to dispense an amount of paint based on CMYK 
}