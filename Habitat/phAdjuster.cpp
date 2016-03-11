#include "phAdjuster.h"
#include "globalMtx.h"
#include "i2c_iO.h"
#include "HabitatConfiguration.h"


#include <wiringPiI2C.h>
#include <cmath>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <fstream>

#define DEFAULT_ADDR 0x60

using namespace std;

std::vector<phAdjuster*> phAdjusters;


phAdjuster::phAdjuster(int slotPosition)
{
	this->slotPosition = slotPosition;
	disconnected = false;
	
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);	
	
	if (wiringPiI2CWriteReg8(deviceID, PCA9685_MODE1, 0x0) == -1)
	{
		disconnected = true;
	}
	busMtx.unlock();
	
	initPHAdjuster();
	stopPump(ADJUSTER_ONE, PH_UP);
	stopPump(ADJUSTER_ONE, PH_DOWN);
	stopPump(ADJUSTER_TWO, PH_UP);
	stopPump(ADJUSTER_TWO, PH_DOWN);
	
	for (int i = 0; i < 2; i++)
	{
		adjOperating[i] = false;
		targetPHVal[i] = 0;
		adjMode[i] = 0;
		linkedPHSensorSlot[i] = -1;
	}	
}


phAdjuster::~phAdjuster()
{
}

void phAdjuster::initPHAdjuster()
{
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	
	int oldmode = wiringPiI2CReadReg8(deviceID, PCA9685_MODE1);
	wiringPiI2CWriteReg8(deviceID, PCA9685_MODE1, oldmode | 0xa1); //turn on register auto increment (e.g. write 16 Bits at once instead of 8 Bits twice)
	
	busMtx.unlock();	
}



//void phAdjuster::setPWMFreq()
//{
//	float freq = 1600 * 0.9;
//	float osciClock = 25000000;
//	int prescale;
//	int oldmode;
//	int newmode;
//	
//	osciClock /= 4096;
//	osciClock /= freq;
//	osciClock -= 1;
//	
//	prescale = floor(osciClock + 0.5);
//		
//	busMtx.lock();
//	oldmode = wiringPiI2CReadReg8(deviceID, PCA9685_MODE1);
//	newmode = (oldmode & 0x7F) | 0x10;
//	
//	wiringPiI2CWriteReg8(deviceID, PCA9685_MODE1, newmode); //sleep or else prescale register is blocked
//	wiringPiI2CWriteReg8(deviceID, PCA9685_PRESCALE, prescale); //write the prescale
//	busMtx.unlock();
//}


void phAdjuster::getMotor(int adjusterID, int motorID)
{
	if (adjusterID == ADJUSTER_ONE)
	{
		if (motorID == PH_DOWN)
		{
			PWMPin = 8;
			IN1Pin = 10;
			IN2Pin = 9;
		}
		else if (motorID == PH_UP)
		{
			PWMPin = 13;
			IN1Pin = 12;
			IN2Pin = 11;
		}
	}
	else if (adjusterID == ADJUSTER_TWO)
	{
		if (motorID == PH_DOWN)
		{
			PWMPin = 2;
			IN1Pin = 4;
			IN2Pin = 3;
		}
		else if (motorID == PH_UP)
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
	setSpeed(speedVal * 16);
	
	//vorwärts (anders rum rückwärts)
	setPin(IN2Pin, LOW);
	setPin(IN1Pin, HIGH);	
}


bool phAdjuster::stopPump(int adjusterID, int motorID)
{
	getMotor(adjusterID, motorID);
	
	setPin(IN1Pin, LOW);
	setPin(IN2Pin, LOW);
}


void phAdjuster::setPWM(int pin, int on, int off)
{	
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	wiringPiI2CWriteReg16(deviceID, LED0_ON + 4*pin, on);	
	wiringPiI2CWriteReg16(deviceID, LED0_OFF + 4*pin, off);
	busMtx.unlock();
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



void phAdjuster::loadSettingsIfExist(int deviceObjectID)
{
	ifstream inFile(getFilepath(), ios::in | ios::binary);
	if (inFile.good())
	{
		inFile.read((char *) phAdjusters[deviceObjectID], sizeof(phAdjuster));
		inFile.close();
	}
}


//simple dump of the object variables(settings) as a binary file. Fine because the object just holds primitiv data types. 
//Not working, if you add pointer variables, but not needed for now
void phAdjuster::saveSettings()
{
	int deviceObjectID = deviceObjectIdList[slotPosition - 1];
	
	ofstream outFile(getFilepath(), ios::out | ios::binary);
	outFile.write((char *) phAdjusters[deviceObjectID], sizeof(phAdjuster));
	outFile.close();
}


std::string phAdjuster::getFilepath()
{
	char buff[50];
	snprintf(buff, sizeof(buff), "device_settings/slot_%d_settings.bin", slotPosition);
	string path = buff;
	
	return path;
}


void phAdjuster::setOperatingStatus(int adjusterID, bool active)
{
	adjOperating[adjusterID - 1] = active;
	saveSettings();
}


bool phAdjuster::getOperatingStatus(int adjusterID)
{
	return adjOperating[adjusterID - 1];
}


void phAdjuster::setTargetPHVal(float phVal, int adjusterID)
{
	targetPHVal[adjusterID - 1] = phVal;
	saveSettings();
}


float phAdjuster::getTargetPHVal(int adjusterID)
{
	return targetPHVal[adjusterID - 1];
}


void phAdjuster::assignToPHSlot(int slotPosition, int adjusterID)
{
	linkedPHSensorSlot[adjusterID - 1] = slotPosition;
	saveSettings();
}


int phAdjuster::getAssignedPHSlot(int adjusterID)
{
	return linkedPHSensorSlot[adjusterID - 1];
}


void phAdjuster::setMode(int adjusterID, int mode)
{	
	if (mode == INTERVAL)
	{
		dateTillNextTick = time(NULL);
		struct tm *date = localtime(&dateTillNextTick);
		date->tm_mday += 1;
		date->tm_isdst = -1;
		dateTillNextTick = mktime(date);
		targetPHVal[adjusterID] = upperPHLimit[adjusterID];
		increaseFlag[adjusterID] = false;
	}
	adjMode[adjusterID - 1] = mode;
	saveSettings();
}


int phAdjuster::getMode(int adjusterID)
{
	return adjMode[adjusterID - 1];
}


bool phAdjuster::checkConnection()
{
	return disconnected;
}


int phAdjuster::getSlotPosition()
{
	return slotPosition;
}

float phAdjuster::getLowerPHLimit(int adjusterID)
{
	return lowerPHLimit[adjusterID];
}

void phAdjuster::setLowerPHLimit(int adjusterID, float phVal)
{
	lowerPHLimit[adjusterID] = phVal;
}


float phAdjuster::getUpperPHLimit(int adjusterID)
{
	return upperPHLimit[adjusterID];
}


void phAdjuster::setUpperPHLimit(int adjusterID, float phVal)
{
	upperPHLimit[adjusterID] = phVal;
}


bool phAdjuster::getIncreaseFlag(int adjusterID)
{
	return increaseFlag[adjusterID];
}


void phAdjuster::setIncreaseFlag(int adjusterID, bool increase)
{
	increaseFlag[adjusterID] = increase;
}


time_t phAdjuster::getDateForNextTick()
{
	return dateTillNextTick;	
}


void phAdjuster::setDateForNextTick(time_t nextDate)
{
	dateTillNextTick = nextDate;
}

