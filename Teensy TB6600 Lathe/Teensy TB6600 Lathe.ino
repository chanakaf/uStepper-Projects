/*
    Name:       Teensy TB6600 Lathe.ino
    Created:	4/3/2021 7:38:31 PM
    Author:     CAD-PC\AF
*/


#include <AccelStepper.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(128, 64, &Wire, -1);

int buttonValue = 0;
int potValue = 0;
int STATE = 0;
int speed = 0;
int pos = 0;

void setup() {

    Serial.begin(115200);

    stepper.setMaxSpeed(500);                   // Set the maximum speed and acceleration
    stepper.setSpeed(-500);
                                                //stepper.setAcceleration(3000);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // Display setup and static text

    delay(1000);
    display.clearDisplay();

    display.setTextSize(2);
    display.setTextColor(WHITE);

    display.setCursor(35, 0);
    display.println("SPEED");       // Display static text

    display.setCursor(95, 20);
    display.println("in");          // Display static text

    display.setCursor(90, 45);
    display.println("min");         // Display static text

    display.drawFastHLine(90, 39, 38, WHITE);
    display.drawFastHLine(90, 40, 38, WHITE);

    display.display();
}

void loop() {
    static uint32_t lastPrint = millis();
    readSwitch();
    //    debugPrint();

    /*
        if (STATE == 1) {
     //       stepper.setRPM(-speed);   //Move the motor forward
        }
        if (STATE == 2) {
     //       stepper.setRPM(speed);    //Move the mortor reverse
        }
        if (STATE == 0) {
    //        stepper.setRPM(0);        // Stop the motor
        }


    */
    //stepper.setSpeed(-500);
    stepper.runSpeed();
    //pos = stepper.currentPosition();
    //pos = pos / 1000;

    if (millis() - lastPrint > 1000) {
        lastPrint = millis();
        debugPrint();
    
    display.setTextSize(4);
    display.setCursor(5, 30);
    display.print("888");       //use variable to display speed, without ""
    display.display();
}

}

void readSwitch() {                                     //creates the function to read the switch states
    buttonValue = analogRead(A0);                       //read the analog pin the switches are connected to 
    potValue = analogRead(A1);                          //read the analog pin the potentiometer is connected to 
    speed = potValue / 3.4;

    if (buttonValue >= 340 && buttonValue <= 365) {     //if the analog reading is within a certain range we know which button was pressed and set the state accordingly
        STATE = 1;
 //       delay(10);                                      //delay for debounce purposes
    }

    if (buttonValue >= 685 && buttonValue <= 720) {     //if the analog reading is within a certain range we know which button was pressed and set the state accordingly
        STATE = 2;
 //       delay(10);                                      //delay for debounce purposes
    }

    if (buttonValue <= 310) {                           //if the analog reading is within a certain range we know which button was pressed and set the state accordingly
        STATE = 0;
 //       delay(10);                                      //delay for debounce purposes
    }

 //    delay(1);                                          //delay for timeing purposes
}

void debugPrint() {
    Serial.println("Switch State:" + String(STATE) + "," 
        + "Switch Value:" + String(buttonValue) + "," 
        + "Pot Value:" + String(potValue) + "," 
        + "Position Value:" + String(pos) + "\n");
}