#include <Wire.h>
#include "DFRobot_TCS34725.h"
#include "LiquidCrystal.h"
//#include

// syringes subsystem
const int syringeMotorCyanPin = 2;
const int syringeMotorMagentaPin = 3;
const int syringeMotorYellowPin = 4;
const int syringeMotorKeyPin = 5;

// mixer subsystem
const int mixerMotorPin = 6;

// ui/display subsystem
const int button0Pin = A0;
const int button1Pin = A1;
const int button2Pin = A2;
const int button3Pin = A3;

int button0 = 0;
int button1 = 0;
int button2 = 0;
int button3 = 0;

int currScreen = 0;

char currHex[] = "ffffff";

LiquidCrystal lcd (13, 12, 11, 10 ,9 ,8);

// color sensor subsystem
const int colorSensorPin0 = A4;
const int colorSensorPin1 = A5;

DFRobot_TCS34725 tcs = DFRobot_TCS34725(&Wire, TCS34725_ADDRESS,TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

uint16_t R, G, B, Clear;
uint8_t R8, G8, B8;
int C, M, Y, K;
// Changes pins later




//_____________________________________________________________________________________________________________________________
void setup() {
  Serial.begin(115200);
  Serial.println("Color View Test!");

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } 
  else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  pinMode(button0Pin, INPUT_PULLUP);
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  	
  lcd.begin(16,2);
	lcd.clear();
  lcd.setCursor(0,0);
}




//_____________________________________________________________________________________________________________________________
void loop() {


  /* This is for testing variables
  Serial.print("C");
  Serial.println(C); // Prints the sensor reading and a newline
  delay(1000); // Wait for a second
  C = C + 35;
  */

  // main switch-case function; each case accounts for its respective screen
  switch(currScreen) {
      case 0:
        // HOME SCREEN
      	Serial.print("Home screen code start ");
      	Serial.println(currScreen);
      
		    lcd.setCursor(0,0);
      	lcd.print(currHex);
      	
      	lcd.setCursor(12,0);
      	lcd.print("HOME");
      	
      	lcd.setCursor(0,1);
      	lcd.print("SCN HEX MIX SET");
      	
      	currScreen = getCurrScreen(0);
        break;

      case 1:
        // RGB SCANNING SCREEN
      	Serial.print("Scanning screen code start ");
      	Serial.println(currScreen);
      
		    lcd.setCursor(0,0);
      	lcd.print(currHex);
      	
      	// there will be a getCurrHex() function running here
      	// it will constantly update currHex and refresh the screen
        // pressing the SEL button will lock in the hex code currently on screen and stop the scanner
      	
      	lcd.setCursor(0,1);
      	lcd.print("SEL BACK");
      
      	currScreen = getCurrScreen(1);
        break;

      case 2:
        // HEXADECIMAL LOOKUP SCREEN
      	Serial.print("Hex lookup screen code start ");
      	Serial.println(currScreen);
      
		    lcd.setCursor(0,0);
      	lcd.print(currHex);
      	
      	// there will be a lookupCurrHex() function that will
      	// handle manual lookup of previous hexadecimal codes
      
      	lcd.setCursor(0,1);
      	lcd.print("SCR SEL ENT BACK");
      
      	currScreen = getCurrScreen(2);
        break;

      case 3:
        // HEXADECIMAL ENTRY SCREEN
      	Serial.print("Hex entry screen code start ");
      	Serial.println(currScreen);
      
		    lcd.setCursor(0,0);
      	lcd.print(currHex);
      	
      	// there will be a enterCurrHex() function that will
      	// handle manual entry of a new hexadecimal code
      	
      	lcd.setCursor(0,1);
      	lcd.print("SCR NEX SEL BACK");
      
      	currScreen = getCurrScreen(3);
        break;

      case 4:
        // MIXING SETUP SCREEN
      	Serial.print("Mixing setup screen code start ");
      	Serial.println(currScreen);
      
		    lcd.setCursor(0,0);
      	lcd.print("X MIN X SEC");
      	
      	// there will be a prepMix() function that will
      	// handle manual entry of mixing time
      	// this will feed into the larger mix() function
      	// taken care of by the mixer subsystem
      	
      	lcd.setCursor(0,1);
      	lcd.print("MIN SEC MIX BACK");
      
      	currScreen = getCurrScreen(4);
        break;

      case 5:
        // MIXING PROCESS SCREEN
		    Serial.print("Mixing process screen code start ");
      	Serial.println(currScreen);
      
      	lcd.setCursor(0,0);
      	lcd.print("TIMER TICKS..");
      	
      	// there will be a timer function that handles the
      	// counting on this screen and stops when done
      	
      	lcd.setCursor(0,1);
      	lcd.print("STOP CON BACK");
      
      	currScreen = getCurrScreen(5);
        break;

      case 6:
        // SETTINGS SCREEN
      	Serial.print("Settings screen code start ");
      	Serial.println(currScreen);
      
		    lcd.setCursor(0,0);
      	lcd.print("SETTINGS");
      	
      	// there will be options to change settings on this screen
      	
      	lcd.setCursor(0,1);
      	lcd.print("XXX XXX XXX BACK");
      
      	currScreen = getCurrScreen(6);
        break;

  	}
}
// end of void loop()






// UI/DISPLAY SUBSYSTEM_____________________________________________________________________________________________________________________________
int getCurrScreen(int prevScreen)
{
  // handles button inputs and button debounce
  // returns the appropriate screen number to currScreen

 	if (prevScreen == 0)
    {
      	button0 = digitalRead(button0Pin);
      	if (!button0) { while(!digitalRead(button0Pin)); lcd.clear(); return 1; }
      	button1 = digitalRead(button1Pin);
      	if (!button1) { while(!digitalRead(button1Pin)); lcd.clear(); return 2; }
      	button2 = digitalRead(button2Pin);
      	if (!button2) { while(!digitalRead(button2Pin)); lcd.clear(); return 4; }
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 6; }
    }
  	if (prevScreen == 1)
    {
      	button1 = digitalRead(button1Pin);
      	if (!button1) { while(!digitalRead(button1Pin)); lcd.clear(); return 0; }
    }
  	if (prevScreen == 2)
    {
      	button2 = digitalRead(button2Pin);
      	if (!button2) { while(!digitalRead(button2Pin)); lcd.clear(); return 3; }
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 0; }
    }
  	if (prevScreen == 3)
    {
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 0; }
    }
  	if (prevScreen == 4)
    {
      	button2 = digitalRead(button2Pin);
      	if (!button2) { while(!digitalRead(button2Pin)); lcd.clear(); return 5; }
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 0; }
    }
  	if (prevScreen == 5)
    {
      	button2 = digitalRead(button2Pin);
      	if (!button2) { while(!digitalRead(button2Pin)); lcd.clear(); return 0; }
    }
  	if (prevScreen == 6)
    {
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 0; }
    }
  	
  	return prevScreen;
}

char getCurrHex(char prevHex) 
{

  return prevHex;
}

void lookupCurrHex()
{

}

void enterCurrHex()
{

}
// end ui/display functions




// COLOR SENSOR SUBSYSTEM_____________________________________________________________________________________________________________________________
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

// Convert raw TCS34725 readings into 0–255 RGB
void rgbTo255(uint16_t R, uint16_t G, uint16_t B, uint16_t C, uint8_t &R8, uint8_t &G8, uint8_t &B8) {
  // Avoid divide-by-zero
  if (C == 0) {
    R8 = G8 = B8 = 0;
    return;
  }

  // Normalize each channel by the clear value
  float Rn = (float)R / (float)C;
  float Gn = (float)G / (float)C;
  float Bn = (float)B / (float)C;

  // Scale to 0–255
  Rn *= 255.0f;
  Gn *= 255.0f;
  Bn *= 255.0f;

  // Clamp to valid range
  R8 = (uint8_t)constrain(Rn, 0, 255);
  G8 = (uint8_t)constrain(Gn, 0, 255);
  B8 = (uint8_t)constrain(Bn, 0, 255);
}
  // example use: 
  /*
  uint16_t r, g, b, c;
  uint8_t R8, G8, B8;

  tcs.getRGBC(&r, &g, &b, &c);

  convertTo255(r, g, b, c, R8, G8, B8);

  Serial.print("RGB(255): ");
  Serial.print(R8); Serial.print(", ");
  Serial.print(G8); Serial.print(", ");
  Serial.print(B8); Serial.println();
  */

void rgbToCMYK() {
  K = 1 - max(max(R, G), max(G, B));
  C = (1-R-K)/(1-K);
  M = (1-G-K)/(1-K);
  Y = (1-B-K)/(1-K);
}
// end color sensor functions




// SYRINGES SUBSYSTEM_____________________________________________________________________________________________________________________________
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
// end syringes functions




// MIXER SUBSYSTEM_____________________________________________________________________________________________________________________________
void prepMix()
{
  // handles manual entry of mixing time

}

void mix()
{
  // handles physical mixing process, and stopping and starting the mixer
  // works with the timer() function

}

void timer()
{
  // encapsulates the timer

}
// end mixer functions
