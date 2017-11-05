#ifndef Pump_h
#define Pump_h

#include "Arduino.h"

class Pump
{
  public:
    Pump(int pin);
	void Open(int minLvl);
	void Close(void);
	void ForceClose(long);
	boolean IsOpen(void);
  private:
    int _pin;
	bool _status;
	int _minLevel;
};

#endif