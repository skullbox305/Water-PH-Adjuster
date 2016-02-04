#include "phSensor.h"
#include "AtlasScientific_i2c_iO.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <stdlib.h>

using namespace std;

phSensor::phSensor(int busAddr)
{
	if (checkIfAddressIsFree(busAddr))
	{
		busAddress = busAddr;
		deviceId = initDevice(busAddr);
		if (deviceId  < 0)
		{
		//exception
		}		
	}
}


phSensor::~phSensor()
{
}


int phSensor::getBusAddress()
{
	return busAddress;
}


bool phSensor::setNewBusAddress(int newAddr)
{
	bool res = false;
	
	if (newAddr > 0 && newAddr < 128)
	{
		char buffer[10];
		sprintf(buffer, "I2c,%d", newAddr);
	
		if (writeI2C(string(buffer), deviceId))
		{
			sleep(3);
			
			if (checkIfAddressIsFree(newAddr))
			{
				if (initDevice(newAddr))
				{				
					busAddress = newAddr;
					res = true;				
				}
			}
		}
	}	
	return res;
}

// >= 0 = value, -1 = bad reading
float phSensor::getTempCompensation()
{
	float tempCompensation;
	if (writeI2C("T,?", deviceId))
	{
		usleep(300000); //wait 300 microseconds for response

		string result;
		if (readI2C(result, deviceId))
		{
			
			if (sscanf(result.substr(3).c_str(), "%f", &tempCompensation) != 1) 
			{
				cout << "Bad reading: " << result << endl;
				tempCompensation = -1;
			}			
		} 		
	}
	return tempCompensation;
}


bool phSensor::setTempCompensation(float newTemp)
{
	bool res = false;
	char buffer[10];
	sprintf(buffer, "T,%f", newTemp);
	
	if (writeI2C(string(buffer), deviceId))
	{
		usleep(300000); //wait 300 microseconds for response

		string reading;
		if (readI2C(reading, deviceId))
		{
			res = true;
		}
	}
	return res;
}

//-1 = fehler, 0 = keine, 1 = Ein-, 2 = Zwei, 3 = Dreipunkt Kalibrierung
int phSensor::getCalibrationStatus()
{
	int result = -1;
	if (writeI2C("Cal,?", deviceId))
	{
		usleep(300000);

		string reading;
		if (readI2C(reading, deviceId))
		{
			if (isdigit(reading[5]))
			{
				result = reading[5] - '0';
			}
		}
	}
	return result;
}


bool phSensor::clearCalibration()
{
	bool res = false;
	if (writeI2C("Cal,clear", deviceId))
	{
		usleep(300000); //wait 300ms for response

		string reading;
		if (readI2C(reading, deviceId))
		{
			res = true;
		}
	}
	return res;
}

bool phSensor::calibration(std::string cmd, float phVal)
{
	bool res = false;
	char buffer[12];
	sprintf(buffer, "%s,%f", cmd.c_str(), phVal);
	
	if (writeI2C(string(buffer), deviceId))
	{
		usleep(1600000);//wait 1.6 sec for response

		string reading;
		if (readI2C(reading, deviceId))
		{
			res = true;
		}
	}
	return res;
}


bool phSensor::lowpointCalibration(float phVal)
{
	return calibration("Cal,low", phVal);
}

bool phSensor::midpointCalibration(float phVal)
{
	return calibration("Cal,mid", phVal);
}


bool phSensor::highpointCalibration(float phVal)
{
	return calibration("Cal,high", phVal);
}

// >= 0 = PH Value, -1 = bad reading
float phSensor::getNewPHReading()
{
	if (writeI2C("R", deviceId))
	{
		sleep(1); //wait 1 sec for response

		string result;
		if (readI2C(result, deviceId))
		{
			if (sscanf(result.c_str(), "%f", &lastPHValue) != 1) 
			{
				cout << "Bad reading: " << result << endl;
				lastPHValue = -1;
			}
		} 		
	}
	return lastPHValue;
}


bool phSensor::startSleepmode()
{
	bool res = false;
	if (writeI2C("SLEEP", deviceId))
	{
		usleep(300000); //wait 300ms for instruction
		res = true;
	}
	return res;
}


bool phSensor::getDeviceInfo(std::string &info)
{
	bool res = false;
	char buffer[30];
	
	if (writeI2C("I", deviceId))
	{
		usleep(300000); //wait 300ms for instruction

		if (readI2C(info, deviceId))
		{
			sprintf(buffer, "%s%s%s%s", "Device: ", info.substr(3, 2).c_str(), "  Firmware: ", info.substr(6, 3).c_str());
			info = string(buffer);
			res = true;
		} 		
	}
	return res;
}


bool phSensor::getSlope(float &acidCalibration, float &baseCalibration)
{
	bool res = false;
	if (writeI2C("SLOPE,?", deviceId))
	{
		usleep(300000); //wait 300ms for instruction

		string result;
		if (readI2C(result, deviceId))
		{
			string acid = result.substr(7, 4).c_str();
			string base = result.substr(12, 4).c_str();
			if (sscanf(acid.c_str(), "%f", &acidCalibration) && sscanf(base.c_str(), "%f", &baseCalibration))
			{
				res = true;
			}			
		}
	}
	return res;	
}


float phSensor::getLastPHReading()
{
	return lastPHValue;
}
