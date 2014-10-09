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
    setDisplay(ERROR_DATA, 4);
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
  sendData(1, 0);
}

void RecieverUI::setDisplayToString(const char* string, const word dots, const byte pos, const byte font[])
{
  for (int i = 0; i < 4 - pos; i++) {
  	if (string[i] != '\0') {
	  sendChar(i + pos, font[string[i] - 32], (dots & (1 << (3 - i))) != 0);
	} else {
	  break;
	}
  }
}

void RecieverUI::setDisplayToString(const String string, const word dots, const byte pos, const byte font[])
{
  int stringLength = string.length();

  for (int i = 0; i < 4 - pos; i++) {
    if (i < stringLength) {
      sendChar(i + pos, font[string.charAt(i) - 32], (dots & (1 << (3 - i))) != 0);
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
    digitalWrite(dataPin, (data >> i) & 1 ? HIGH : LOW);
    digitalWrite(clockPin, HIGH);
  }
}

byte RecieverUI::receive()
{
  byte temp = 0;

  // Pull-up on
  pinMode(dataPin, INPUT);
  digitalWrite(dataPin, HIGH);

  for (int i = 0; i < 8; i++) { //8 buttons
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

void RecieverUI::setDisplayToHexNumber(unsigned long number, byte dots, boolean leadingZeros,
        const byte numberFont[])
{
  for (int i = 0; i < 4; i++) {
        if (!leadingZeros && number == 0) {
                clearDisplayDigit(3 - i, (dots & (1 << i)) != 0);
        } else {
                setDisplayDigit(number & 0xF, 3 - i, (dots & (1 << i)) != 0, numberFont);
                number >>= 4;
    }
  }
}

void RecieverUI::setDisplayToDecNumberAt(unsigned long number, byte dots, byte startingPos, boolean leadingZeros,
        const byte numberFont[])
{
  if (number > 99999999L) {
    setDisplayToError();
  } else {
    for (int i = 0; i < 4 - startingPos; i++) {
      if (number != 0) {
        setDisplayDigit(number % 10, 3 - i, (dots & (1 << i)) != 0, numberFont);
        number /= 10;
      } else {
                if (leadingZeros) {
                  setDisplayDigit(0, 3 - i, (dots & (1 << i)) != 0, numberFont);
                } else {
                  clearDisplayDigit(3 - i, (dots & (1 << i)) != 0);
                }
      }
    }
  }
}

void RecieverUI::setDisplayToDecNumber(unsigned long number, byte dots, boolean leadingZeros,
        const byte numberFont[])
{
        setDisplayToDecNumberAt(number, dots, 0, leadingZeros, numberFont);
}


void TM1638::setDisplayToSignedDecNumber(signed long number, byte dots, boolean leadingZeros,
                const byte numberFont[])
{
        if (number >= 0) {
                setDisplayToDecNumberAt(number, dots, 0, leadingZeros, numberFont);
        } else {
                if (-number > 9999999L) {
                    setDisplayToError();
                } else {
                        setDisplayToDecNumberAt(-number, dots, 1, leadingZeros, numberFont);
                        sendChar(0, MINUS, (dots & (0x80)) != 0);
                }
        }
}

void RecieverUI::setDisplayToBinNumber(byte number, byte dots, const byte numberFont[])
{
  for (int i = 0; i < 4; i++) {
    setDisplayDigit((number & (1 << i)) == 0 ? 0 : 1, 3 - i, (dots & (1 << i)) != 0, numberFont);
  }
}

byte RecieverUI::getButtons(void)
{
  byte keys = 0;

  digitalWrite(strobePin, LOW);
  send(0x42);
  for (int i = 0; i < 4; i++) { // we have 8 buttons; do we need to change the 4 to 8 ??
    keys |= receive() << i;
  }
  digitalWrite(strobePin, HIGH);

  return keys;
}

void RecieverUI::sendChar(byte pos, byte data, boolean dot)
{
        sendData(pos << 1, data | (dot ? 0b10000000 : 0)); // what's the meaning of 0b10000000 ??
}

