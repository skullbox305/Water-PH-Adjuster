#include "phAdjuster.h"
#include "globalMtx.h"

#include <wiringPiI2C.h>
#include <cmath>
#include <unistd.h>

std::vector<phAdjuster*> phAdjusters;


phAdjuster::phAdjuster(int slotPosition)
{
	this->slotPosition = slotPosition;
	deviceID = wiringPiI2CSetup(0x60);
	wiringPiI2CWriteReg8(deviceID, PCA9685_MODE1, 0x0);
	setPWMFreq();
	
//	for (int i = 0; i < 16; i++)
//	{
//		setPWM(i, 0, 0);
//	}
}


phAdjuster::~phAdjuster()
{
}


void phAdjuster::setPWMFreq()
{
	float freq = 1600 * 0.9;
	float osciClock = 25000000;
	int prescale;
	int oldmode;
	int newmode;
	
	osciClock /= 4096;
	osciClock /= freq;
	osciClock -= 1;
	
	prescale = floor(osciClock + 0.5);
		
	oldmode = wiringPiI2CReadReg8(deviceID, PCA9685_MODE1);
	newmode = (oldmode & 0x7F) | 0x10;
	
	wiringPiI2CWriteReg8(deviceID, PCA9685_MODE1, newmode);
	wiringPiI2CReadReg8(deviceID, PCA9685_MODE1);	
	wiringPiI2CWriteReg8(deviceID, PCA9685_PRESCALE, prescale);
	wiringPiI2CWriteReg8(deviceID, PCA9685_MODE1, oldmode);
	usleep(5000);
	wiringPiI2CWriteReg8(deviceID, PCA9685_MODE1, oldmode | 0xa1);
}


void phAdjuster::getMotor(int adjusterID, int motorID)
{
	if (adjusterID == adjusterOne)
	{
		if (motorID == phUP)
		{
			PWMPin = 8;
			IN1Pin = 10;
			IN2Pin = 9;
		}
		else if (motorID == phDown)
		{
			PWMPin = 13;
			IN1Pin = 11;
			IN2Pin = 12;
		}
	}
	else if (adjusterID == adjusterTwo)
	{
		if (motorID == phUP)
		{
			PWMPin = 2;
			IN1Pin = 4;
			IN2Pin = 3;
		}
		else if (motorID == phDown)
		{
			PWMPin = 7;
			IN1Pin = 5;
			IN2Pin = 6;
		}
	}	
}


bool phAdjuster::startPump(int adjusterID, int motorID, int speedVal)
{
	getMotor(adjusterID, motorID);	
	setSpeed(speedVal*16);
	
	//vorwärts (anders rum rückwärts)
	setPin(IN2Pin, LOW);
	setPin(IN1Pin, HIGH);	
}


bool phAdjuster::stopPump(int adjusterID, int motorID)
{
	setPin(IN1Pin, LOW);
	setPin(IN2Pin, LOW);
}


void phAdjuster::setPWM(int pin, int on, int off)
{	
	wiringPiI2CWriteReg16(deviceID, LED0_ON_L + 4*pin, on);	
	wiringPiI2CWriteReg16(deviceID, LED0_OFF_L + 4*pin, off);
}

void phAdjuster::setSpeed(int speedVal)
{
	if (speedVal > 4095)
	{
		setPWM(PWMPin, 4096, 0);
	}
	else
	{
		setPWM(PWMPin, 0, speedVal);
	}
}

void phAdjuster::setPin(int pin, bool val)
{
	if (val == LOW)
	{
		setPWM(pin, 0, 0);
	}
	else
	{
		setPWM(pin, 4096, 0);
	}
}


