/* This is a demonstration for the RecieverUI library
 * by Oussema HARBI <oussema.elharbi@gmail.com>
 * feel free to use, modify, update as you like, but plz keep me informed with your updates.
 * This sketch demonstrates some functions on the display and after that it will
 * write Eror on the display and keep blinking it;
 * You can open the serial port, once display demo functions are done, you can press buttons on the UI or use an IR remote control and you ll be able to see the button you pressed via serial log
 */

#include "RecieverUI.h"
#include <IRremote.h>

RecieverUI ui(7,8,9);

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);

decode_results results;


byte values[4]={9, 3, 4, 6};

const char scrollingText[] = "- SCROLLING TEXT - ";
int j=0;

void setup()
{
  Serial.begin(9600);
  
  irrecv.enableIRIn(); // Start the receiver
  
  ui.setupDisplay( true, 7); //Maximum brightness
  ui.setDisplayToString("CAN5");
  delay(1000);
  ui.setDisplayDigit(7, 3, true);
  delay(1000);
  ui.clearDisplayDigit( 3, true);
  delay(1000);
  ui.clearDisplay();
  delay(1000);
  ui.setDisplay (values, 4); // Does not work
  delay(1000);
  ui.setDisplayToHexNumber(0xC2, 0);
  delay(1000);
  ui.setDisplayToSignedDecNumber( -3, true);
  delay(1000);
  ui.clearDisplay();
  delay(1000);
  for (int i=0; i < 60; i++)
  {
   j=i % (sizeof(scrollingText) - 3); 
   const char* startPos = scrollingText + j;
   ui.setDisplayToString( startPos);
   delay(250);
  }
  ui.setDisplayToError(); // For some reason 
  
}

byte c=0;

void loop()
{
  delay(500);
  ui.setupDisplay( true, 0);
  c= ui.getButtons();
  if (c != 0)
    Serial.println( c, DEC);
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
  delay(500);
  ui.setupDisplay( true, 7);
  c= ui.getButtons();
  if (c != 0)
    Serial.println( c, DEC);
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
}


