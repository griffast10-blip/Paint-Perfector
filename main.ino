// data to be sent, guarantees 32 bytes total
struct DataStruct
{
    uint8_t C;              //  8
    uint8_t M;              //  8
    uint8_t Y;              //  8
    uint8_t K;              //  8
                            //------
                            // 32
};

DataStruct StepData = {0};

bool newStepData = false;

#include <Wire.h>
#include "DFRobot_TCS34725.h"
#include "LiquidCrystal.h"
#include <string.h>
#include <math.h>

const byte thisAddress = 8; // these need to be swapped for the slave Arduino
const byte otherAddress = 9;

// timing variables
unsigned long prevUpdateTime = 0;
unsigned long updateInterval = 500;

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

char currHex[8] = {000000};

LiquidCrystal lcd (13, 12, 11, 10, 9, 8);

char hexAlphabet[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
int hexScrollCount = 0;
int currHexPos = 0;

// color sensor subsystem
const int colorSensorPin0 = A4;
const int colorSensorPin1 = A5;

DFRobot_TCS34725 tcs = DFRobot_TCS34725(&Wire, TCS34725_ADDRESS,TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);

uint16_t colorTemp, lux;
float C, M, Y, K;
uint16_t red, green, blue, clear;

float rFact = 1450;
float gFact = 910;
float bFact = 650;

int luxCount = 0;

uint32_t r, g, b;

//_____________________________________________________________________________________________________________________________
void setup()
{
  Serial.begin(9600);
  Serial.println("Begin Setup...");

  // check for RGB sensor
  if (!tcs.begin())
  {
    Serial.println("Sensor not found ... check your connections");
    while(!tcs.begin())
    {
      tcs.begin();
    }
  }

  Serial.println("Sensor Found");
  Serial.println("Starting I2C Master Arduino...");

  // set up I2C
  Wire.begin(thisAddress);

  // define pin modes for interface buttons to counter debounce
  pinMode(button0Pin, INPUT_PULLUP);
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);

  // initialize LCD screen
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  sprintf(currHex, "%02X%02X%02X", 00, 00, 00);
}

//_____________________________________________________________________________________________________________________________
void loop()
{
  // main switch-case function; each case accounts for its respective screen
  switch(currScreen)
  {
      case 0:
        // HOME SCREEN      
		lcd.setCursor(0,0);
        lcd.print("#");
        lcd.setCursor(1,0);
      	lcd.print(currHex);
      	
      	lcd.setCursor(12,0);
      	lcd.print("HOME");
      	
      	lcd.setCursor(0,1);
      	lcd.print("SCN HEX MIX SET");
      	
      	currScreen = getCurrScreen(0);
        break;

      case 1:
        // RGB SCANNING SCREEN
		lcd.setCursor(0,0);
        lcd.print("#");
        lcd.setCursor(1,0);
      	lcd.print(currHex);
      	
      	lcd.setCursor(0,1);
      	lcd.print("SEL BRI DAR BACK");
      
      	currScreen = getCurrScreen(1);
        break;

      case 2:
        // HEXADECIMAL LOOKUP SCREEN | DEPRECATED      
      	currScreen = getCurrScreen(2);
        break;

      case 3:
        // HEXADECIMAL ENTRY SCREEN | handles manual hex color entry and editing
		lcd.setCursor(0,0);
      	lcd.print(currHex);

        lcd.setCursor(10,0);
      	lcd.print("POS:");
        lcd.setCursor(15, 0);
        lcd.print(currHexPos + 1);
      	      	
      	lcd.setCursor(0,1);
      	lcd.print("SCR NEX     BACK");
      
      	currScreen = getCurrScreen(3);
        break;

      case 4:
        // MIXING SCREEN | compiles and sends CMYK color data to slave arduino for dispensing
		lcd.setCursor(0,0);
      	lcd.print("MIX: "); lcd.print(currHex);
      	
      	lcd.setCursor(0,1);
      	lcd.print("(EXT)       BACK");
		// (ext) means the mixer has its own standalone button and operation
      
      	currScreen = getCurrScreen(4);
        break;

      case 5:
        // MIXING PROCESS SCREEN | DEPRECATED
      	currScreen = getCurrScreen(5);
        break;

      case 6:
        // SETTINGS SCREEN | organized so that future settings can be changed
		lcd.setCursor(0,0);
      	lcd.print("SETTINGS");
      	     	
      	lcd.setCursor(0,1);
      	lcd.print("RES         BACK");
      
      	currScreen = getCurrScreen(6);
        break;
  	}
}
// end of void loop()

// UI/DISPLAY SUBSYSTEM_____________________________________________________________________________________________________________________________

// main interface function
// handles button inputs and button debounce, calls processing functions
// returns the appropriate screen number to currScreen
int getCurrScreen(int prevScreen)
{
 	if (prevScreen == 0)
    {
      	button0 = digitalRead(button0Pin);
      	if (!button0) { while(!digitalRead(button0Pin)); lcd.clear(); return 1; }
      	button1 = digitalRead(button1Pin);
      	if (!button1) { while(!digitalRead(button1Pin)); lcd.clear(); return 3; }
      	button2 = digitalRead(button2Pin);
      	if (!button2) { while(!digitalRead(button2Pin)); lcd.clear(); updateDataToSend(1); transmitData(); return 4; }
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 6; }
    }
  	if (prevScreen == 1)
    {
        button0 = digitalRead(button0Pin);
      	if (!button0) { while(!digitalRead(button0Pin)); luxCount = 0; colorSensor(); return 1; }
      	button1 = digitalRead(button1Pin);
      	if (!button1) { while(!digitalRead(button1Pin)); incrementLux(1); return 1; }
        button2 = digitalRead(button2Pin);
      	if (!button2) { while(!digitalRead(button2Pin)); incrementLux(2); return 1; }
        button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); incrementLux(0); lcd.clear(); return 0; }
    }
  	if (prevScreen == 2)
    {
        lcd.clear(); return 0;
    }
  	if (prevScreen == 3)
    {
        button0 = digitalRead(button0Pin);
      	if (!button0) { while(!digitalRead(button0Pin)); scrollHex(); return 3; }
        button1 = digitalRead(button1Pin);
      	if (!button1) { while(!digitalRead(button1Pin)); nextHexPos(); return 3; }
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 0; }
    }
  	if (prevScreen == 4)
    {
        button0 = digitalRead(button0Pin);
      	if (!button0) { while(!digitalRead(button0Pin)); return 4; }
      	button1 = digitalRead(button1Pin);
      	if (!button1) { while(!digitalRead(button1Pin)); return 4; }
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 0; }
    }
  	if (prevScreen == 5)
    {
        lcd.clear(); return 0;
    }
  	if (prevScreen == 6)
    {
        button0 = digitalRead(button0Pin);
      	if (!button0) { while(!digitalRead(button0Pin)); updateDataToSend(0); transmitData(); return 6; }
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 0; }
    }  	
  	return prevScreen;
}

// iterates through the hexadecimal alphabet and displays the current character
void scrollHex()
{
  char currLetter = currHex[currHexPos];

  for (int i = 0; i < 16; i++)
  {
    if (hexAlphabet[i] == currLetter)
    {
      if (i == 15)
      {
        currLetter = hexAlphabet[0];
        currHex[currHexPos] = currLetter;
        return;
      }
      else
      {
        currLetter = hexAlphabet[i + 1];
        currHex[currHexPos] = currLetter;
        return;
      }
    }
  }
}

// iterates through each character of the currHex variable
void nextHexPos()
{
  if (currHexPos == 5)
  {
    currHexPos = 0;
    return;
  }
  else
  {
    currHexPos += 1;
    return;
  }
}
// end ui/display functions

// COLOR SENSOR SUBSYSTEM_____________________________________________________________________________________________________________________________

// captures RGB color data from sensor, calculates lux and color temperature
// calls rgbToHEX() to convert data to hexadecimal for display
void colorSensor()
{
  tcs.getRGBC(&red, &green, &blue, &clear);
  colorTemp = tcs.calculateColortemperature(red, green, blue);
  lux = tcs.calculateLux(red, green, blue);

  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);
  Serial.print("\tColor Temp:\t"); Serial.print(colorTemp); Serial.print("K");
  Serial.print("\tLux:\t"); Serial.print(lux);
  Serial.println("\t");

  rgbToHEX(red, green, blue, clear, lux, currHex);
}

// receives and processes RGB color data for accuracy
// then coverts that data to a hexadecimal string currHex
void rgbToHEX(uint16_t Red, uint16_t Green, uint16_t Blue, uint16_t Clear, uint16_t Lux, char currHex[8])
{
  float brightness;
  if ((Lux == 65535) || (Lux < 20))
  {
    brightness = 0;
  }
  else if (Lux > 625)
  {
    brightness = 5.2;
    rFact = 1800;
  }
  else if (Lux > 300)
  {
    brightness = log((float)(Lux - 20)) / 1.75;
  }
  else
  {
    brightness = log((float)(Lux - 25)) / 2.5;
    bFact = 700;
  }
  
  // Convert raw TCS34725 readings into 0–255 RGB
  if (Clear == 0)  return; 
  
  // Normalize each channel by the clear value and lux
  r = ((uint32_t)Red * 255) / ((uint32_t)Clear * 1.0);
  g = ((uint32_t)Green * 255) / ((uint32_t)Clear * 1.0);
  b = ((uint32_t)Blue * 255) / ((uint32_t)Clear * 1.0);
  
  // map to proper 8-bit RGB range + adjust values to compensate for sensor error
  r = (uint8_t)constrain((r*rFact*brightness) / 1700, 0, 255);
  g = (uint8_t)constrain((g*gFact*brightness) / 1700, 0, 255);
  b = (uint8_t)constrain((b*bFact*brightness) / 1700, 0, 255);

  Serial.println();
  Serial.println("Adjusted + Mapped RGB Normalized");
  Serial.print("R:\t"); Serial.print(r, 4);
  Serial.print("\tG:\t"); Serial.print(g, 4);
  Serial.print("\tB:\t"); Serial.print(b, 4);
  Serial.println("\t");

  // save 8 bit values to a singular hexadecimal variable
  sprintf(currHex, "%02X%02X%02X", (uint8_t)r, (uint8_t)g, (uint8_t)b);

  Serial.println();
  Serial.print("currHex: "); Serial.println(currHex);
  return;
}

// quick button operation to increase or decrease brightness on the scanning screen
void incrementLux(int incType)
{
  if (incType == 0) { luxCount = 0; return; }

  if (incType == 1)
  {
    luxCount += 1;
    Serial.print("Previous Lux: "); Serial.print(lux); Serial.print("\tLux Counter: "); Serial.println(luxCount);
    rgbToHEX(red, green, blue, clear, lux + luxCount, currHex);
    return;
  }
  else
  {
    luxCount -= 1;
    Serial.print("Current Lux: "); Serial.print(lux); Serial.print("\tLux Counter: "); Serial.println(luxCount);
    rgbToHEX(red, green, blue, clear, lux + luxCount, currHex);
    return;
  }
}
// end color sensor functions

// SYRINGES SUBSYSTEM_____________________________________________________________________________________________________________________________

// converts current, post-processing RGB color data to CMYK to be sent to the slave Arduino
void rgbToCMYK()
{
  float rf = r / 255.0;
  float gf = g / 255.0;
  float bf = b / 255.0;

  K = 1.0 - max(rf, max(gf, bf));

  if (K == 1.0)
  {
    C = M = Y = 0.0;
  } 
  else
  {
    C = (1.0 - rf - K) / (1.0 - K);
    M = (1.0 - gf - K) / (1.0 - K);
    Y = (1.0 - bf - K) / (1.0 - K);
  }
}

// transmits CMYK color data to the slave Arduino over I2C
void transmitData()
{
    if (newStepData == true)
	{
        Wire.beginTransmission(otherAddress);
        Wire.write((byte*) &StepData, sizeof(StepData));
        Wire.endTransmission();    // this is what actually sends the data

        // show the data that as been sent
        Serial.println("Sent:");
        Serial.println(StepData.C);
        Serial.println(StepData.M);
        Serial.println(StepData.Y);
        Serial.println(StepData.K);

        newStepData = false;
    }
}

// in mode 0, resets DataStruct to 0
// in mode 1, calls rgbToCMYK() and updates DataStruct with results for transmission
void updateDataToSend(int mode)
{
    if (millis() - prevUpdateTime >= updateInterval) // keeps the I2C bus transmitting at a constant rate in the event of communication spam
	{
        prevUpdateTime = millis();
        if (mode == 0)
        {
            Serial.println("resetting...");
            StepData.C = (uint8_t)(0);
            StepData.M = (uint8_t)(0);
            StepData.Y = (uint8_t)(0);
            StepData.K = (uint8_t)(0);
        }
        else if (newStepData == false) // ensure previous message has been sent
		{ 
            // CMYK data values go HERE to send to slave Arduino
            rgbToCMYK();
            Serial.print("C ==== ");
            Serial.println(C);
            Serial.print("M ==== ");
            Serial.println(M);
            Serial.print("Y ==== ");
            Serial.println(Y);
            Serial.print("K ==== ");
            Serial.println(K);

            StepData.C = (uint8_t)(C * 100);
            StepData.M = (uint8_t)(M * 100);
            StepData.Y = (uint8_t)(Y * 100);
            StepData.K = (uint8_t)(K * 100);
            
            newStepData = true;
        }
    }
}
// end syringe dispensing functions

//_____________________________________________________________________________________________________________________________________________
// for testing: how much free ram do we have?
int freeRam()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
