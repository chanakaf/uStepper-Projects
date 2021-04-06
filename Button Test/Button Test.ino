
#include <uStepperS.h>

uStepperS stepper;

int buttonValue = 0;
int potValue = 0;
int STATE = 0;
int speed = 0;

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    stepper.setup(CLOSEDLOOP, 200);			//Initiate the stepper object to use closed loop control with 200 steps per revolution motor - i.e. 1.8 deg stepper 
    stepper.checkOrientation(30.0);
    stepper.setCurrent(100);				//Set operating current %
    }

void loop()
{
    potValue = analogRead(A1);
    buttonValue = analogRead(A0);   //read the button value FWD(1) or REV(2)
    speed = potValue / 3.75;

    if (buttonValue >= 340 && buttonValue <= 365) {
    STATE = 1;
    stepper.setRPM(-speed);
    }
    else if (buttonValue >= 685 && buttonValue <= 720) {
        STATE = 2;
        stepper.setRPM(speed);
    }
    else if (buttonValue <= 310) {
        STATE = 0;
        stepper.setRPM(0);
    }
 
    Serial.println("Switch State:" + String(STATE) + ","
        + "Switch Value:" + String(buttonValue) + ","
        + "Pot Value:" + String(potValue) + ","
        + "Position Value:" + String(speed) + "\n");
 
    Serial1.print("ST:" + String(STATE) + ","
        + "S:" + String(buttonValue) + ","
        + "P:" + String(potValue)+ ",");
}
