

#include <uStepperS.h>
uStepperS stepper;

void setup()
{
    Serial.begin(9600);
    stepper.setup(CLOSEDLOOP, 200);			//Initiate the stepper object to use closed loop control with 200 steps per revolution motor - i.e. 1.8 deg stepper 
    stepper.checkOrientation(30.0);
    //stepper.setRPM(100);
    stepper.setCurrent(100);					//Set operating current %
}

void loop()
{
    char cmd;

    while (!Serial.available());
    Serial.println("ACK!");
    cmd = Serial.read();
    if (cmd == '1')                      //Run continous clockwise
    {
        stepper.setRPM(-250);
    }

    else if (cmd == '2')                 //Run continous counter clockwise
    {
        stepper.setRPM(250);
    }

    else if (cmd == '3')                 //Stop without deceleration and block motor
    {
        stepper.stop();
    }

}
