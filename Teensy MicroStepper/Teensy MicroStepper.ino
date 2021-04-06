/* Example sketch to control a stepper motor with TB6600 stepper motor driver, 
AccelStepper library and Arduino: acceleration and deceleration. More info: https://www.makerguides.com */

// Include the AccelStepper library:


//#include <timer.h>
#include <iAccelStepper.h>
#include <AccelStepper.h> 

// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1

int pos = 0;

// Create a new instance of the AccelStepper class:
//AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
iAccelStepper stepper;

void setup() {

	stepper.begin(motorInterfaceType, stepPin, dirPin);

	// Set the maximum speed and acceleration:
	stepper.setMaxSpeed(500);
	//stepper.setAcceleration(3000);
	stepper.setSpeed(-500);
}
void loop() {
/*	// Set the target position:
	stepper.moveTo(2000);
	// Run to target position with set speed and acceleration/deceleration:
	stepper.runToPosition();
	pos = stepper.currentPosition();
	delay(1000);
	// Move back to zero:
	stepper.moveTo(0);
	stepper.runToPosition();
	pos = stepper.currentPosition();
	delay(1000);
	*/
	stepper.runSpeed();
}