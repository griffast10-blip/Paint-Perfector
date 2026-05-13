#include <Arduino.h>
#include <Stepper.h>
#include <Wire.h>

//===================
// Using I2C to send and receive structs between two Arduinos
//   SDA is the data connection and SCL is the clock connection
//   On an Uno  SDA is A4 and SCL is A5
//   On an Mega SDA is 20 and SCL is 21
//   GNDs must also be connected
//===================


        // data to be received

struct I2cRxStruct {
    int recStep;               //  2
    byte padding[30];       // 30
                            //------
                            // 32
};

I2cRxStruct rxData;

bool newRxData = false;


// Define motor steps per revolution (28BYJ-48 has 2048 steps/rev with gearbox)
const int STEPS_PER_REV = 2048;

// Pin connections to ULN2003 driver IN1-IN4
const int IN1 = 8;
const int IN2 = 9;
const int IN3 = 10;
const int IN4 = 11;

const byte thisAddress = 9; // these need to be swapped for the other Arduino
const byte otherAddress = 8;

// Create Stepper object
Stepper stepperMotor(STEPS_PER_REV, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(9600);
  Serial.println("Stepper Motor Control with ULN2003 Driver");


  Serial.println("\nStarting I2C Slave demo\n");

  // set up I2C
  Wire.begin(thisAddress); // join i2c bus
  Wire.onReceive(receiveEvent); // register event


  // Set motor speed (RPM)
  stepperMotor.setSpeed(15); // Adjust for smoothness and torque
}

void loop() {
  // Example: rotate one revolution clockwise
  //Serial.println("Rotating CW...");
  //stepperMotor.step(STEPS_PER_REV);

          // this bit checks if a message has been received
  if (newRxData == true) {
      showNewData();
      stepperMotor.step(rxData.recStep);
      newRxData = false;
  }

}


//=============

void showNewData() {

    Serial.print("This just in    ");
    Serial.println(rxData.recStep);
}

//============

        // this function is called by the Wire library when a message is received
void receiveEvent(int numBytesReceived) {

    if (newRxData == false) {
            // copy the data to rxData
        Wire.readBytes( (byte*) &rxData, numBytesReceived);
        newRxData = true;
    }
    else {
            // dump the data
        while(Wire.available() > 0) {
            byte c = Wire.read();
        }
    }
}
