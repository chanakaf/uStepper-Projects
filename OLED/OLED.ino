 
//#include <Adafruit_GFX.h>
//#include <uStepperEncoder.h>
#include <Adafruit_SSD1306.h>
//#include <Wire.h>
#include <uStepperS.h>
#include <SSD1306AsciiWire.h>
#include <SSD1306Ascii.h>


int pos = 0;
int rotations = 0;

//#define SCREEN_WIDTH 128 // OLED display width, in pixels
//#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
uStepperS stepper;
//uStepperEncoder enc;
//Adafruit_SSD1306 display(128, 64, &Wire, -1);
SSD1306AsciiWire display;

void setup() {
    Serial.begin(9600);
    Wire.begin();
    Wire.setClock(400000L);
 // display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.begin(&Adafruit128x64, 0x3D, -1);

    stepper.setup(CLOSEDLOOP, 200);			//Initiate the stepper object to use closed loop control with 200 steps per revolution motor - i.e. 1.8 deg stepper 
    stepper.checkOrientation(30.0);
    stepper.setCurrent(10);				//Set operating current %
    stepper.setRPM(50);
/*

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
*/
    delay(2000);
//  display.clearDisplay();
    display.clear();
    display.setFont(Adafruit5x7);
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
    display.set2X();
    
    display.setCursor(35, 0);
    display.println("SPEED");       // Display static text

    display.setCursor(95, 3);
    display.println("in");          // Display static text
 
    display.setCursor(90, 5);
    display.println("min");         // Display static text
    
    //diplay.drawFastHLine(90,39,38, WHITE);
    //display.drawFastHLine(90,40,38, WHITE);
    
    //display.display();
}

void loop() {
/*    stepper.setRPM(200);
    display.setTextSize(4);
    display.setCursor(5, 30);
    display.print("888");       //use variable to display speed, without ""
    display.display();

    */
    pos = stepper.encoder.getAngle();
    if (pos == 359) {
        rotations++;
    }
    display.set2X();
    display.setCursor(5, 3);
    display.println(rotations);
}


