/*
  Pump.cpp - Library for managing pump relays.
  Created by Marco T.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Pump.h"

Pump::Pump(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin,HIGH);
  _pin = pin;
  _status = false;
  _minLevel = 100;
}

void Pump::Open(int minLvl)
{
  digitalWrite(_pin, LOW);
  _minLevel = minLvl;
  _status = true;
}

void Pump::Close(void)
{
  digitalWrite(_pin, HIGH);
  _status = false;
}

boolean Pump::IsOpen(void)
{
  return _status;
}

void Pump::ForceClose(long water)
{
	if(water >= _minLevel) {
		digitalWrite(_pin, HIGH);
		_status = false;
	}
}