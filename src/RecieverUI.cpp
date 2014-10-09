/* File : Reciever.cpp
 * Desc : RecieverUI class implementation
 *	: This class allows interfacing with a generic Digital Satellite Reciever User Interface hosting :
 *	: 1- A 4 digit 7-Segment display
 *	: 2- An IR reciever LED
 *	: 3- 7 switch buttons
 *	: This library allows showing text on the display, detecting pressed buttons (Simultanous buttons are supported)
 *	: and detecting buttons pressed on a generic IR remote control pointed at the IR reciever
 * Lic  : This library is released under GPLv2 terms; It's heavily based on T16XX library
 * Auth : Oussema HARBI <oussema.elharbi@gmail.com>
 */

#include "RecieverUI.h"

#include "string.h"

RecieverUI::RecieverUI(byte dataPin, byte clockPin, byte strobePin, byte intensity)
{
  this->dataPin = dataPin;
  this->clockPin = clockPin;
  this->strobePin = strobePin;

  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(strobePin, OUTPUT);

  digitalWrite(strobePin, HIGH);
  digitalWrite(clockPin, HIGH);

  sendCommand(0x40);
  sendCommand(0x80 | (8 | min(7, intensity));

  digitalWrite(strobePin, LOW);
  send(0xC0);
  for (int i = 0; i < 16; i++) { //Why is this ?
    send(0x00);
  }
  digitalWrite(strobePin, HIGH);
}

void RecieverUI::setupDisplay(boolean active, byte intensity)
{
  sendCommand(0x80 | (active ? 8 : 0) | min(7, intensity));

  // necessary for the TM1640 // Why is this ?
  digitalWrite(strobePin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(strobePin, HIGH);
}

void RecieverUI::setDisplayDigit(byte digit, byte pos, boolean dot, const byte numberFont[])
{
  sendChar(pos, numberFont[digit & 0xF], dot);
}

void RecieverUI::setDisplayToError()
{
    setDisplay(ERROR_DATA, 8);
}

void RecieverUI::clearDisplayDigit(byte pos, boolean dot)
{
  sendChar(pos, 0, dot);
}

void RecieverUI::setDisplay(const byte values[], unsigned int size)
{
  for (int i = 0; i < size; i++) {
    sendChar(i, values[i], 0);
  }
}

void RecieverUI::clearDisplay()
{
  for (int i = 0; i < displays; i++) {
    sendData(i << 1, 0);
  }
}

void RecieverUI::setDisplayToString(const char* string, const word dots, const byte pos, const byte font[])
{
  for (int i = 0; i < displays - pos; i++) {
  	if (string[i] != '\0') {
	  sendChar(i + pos, font[string[i] - 32], (dots & (1 << (displays - i - 1))) != 0);
	} else {
	  break;
	}
  }
}

void RecieverUI::setDisplayToString(const String string, const word dots, const byte pos, const byte font[])
{
  int stringLength = string.length();

  for (int i = 0; i < displays - pos; i++) {
    if (i < stringLength) {
      sendChar(i + pos, font[string.charAt(i) - 32], (dots & (1 << (displays - i - 1))) != 0);
    } else {
      break;
    }
  }
}

void RecieverUI::sendCommand(byte cmd)
{
  digitalWrite(strobePin, LOW);
  send(cmd);
  digitalWrite(strobePin, HIGH);
}

void RecieverUI::sendData(byte address, byte data)
{
  sendCommand(0x44);
  digitalWrite(strobePin, LOW);
  send(0xC0 | address);
  send(data);
  digitalWrite(strobePin, HIGH);
}

void RecieverUI::send(byte data)
{
  for (int i = 0; i < 8; i++) {
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, data & 1 ? HIGH : LOW);
    data >>= 1;
    digitalWrite(clockPin, HIGH);
  }
}

byte RecieverUI::receive()
{
  byte temp = 0;

  // Pull-up on
  pinMode(dataPin, INPUT);
  digitalWrite(dataPin, HIGH);

  for (int i = 0; i < 8; i++) {
    temp >>= 1;

    digitalWrite(clockPin, LOW);

    if (digitalRead(dataPin)) {
      temp |= 0x80;
    }

    digitalWrite(clockPin, HIGH);
  }

  // Pull-up off
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, LOW);

  return temp;
}

#if !defined(ARDUINO) || ARDUINO < 100
// empty implementation instead of pure virtual for older Arduino IDE
void RecieverUI::sendChar(byte pos, byte data, boolean dot) {}
#endif

