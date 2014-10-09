#include "RecieverUI.h"

RecieverUI ui(7,8,9);

byte values[4]={9, 3, 4, 6};

const char scrollingText[] = "- SCROLLING TEXT - ";
int j=0;

void setup()
{
  Serial.begin(9600);
  ui.setupDisplay( true, 7); //Maximum brightness
  ui.setDisplayToString("2608");
  delay(1000);
  ui.setDisplayDigit(7, 1, true);
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

void loop()
{
  delay(1000);
  ui.setupDisplay( true, 0);
  Serial.println(ui.getButtons(), DEC);
  delay(1000);
  ui.setupDisplay( true, 7);
  Serial.println(ui.getButtons(), DEC);
}

