#include "phSensor.h"
#include "AtlasScientific_i2c_iO.h"

#include <iostream>
#include <unistd.h>
#include <string>
#include <stdexcept>

using namespace std;

#define factoryDefaultAddress 0x63
#define firstPHAddress 0x3
#define secondPHAddress 0x4

phSensor::phSensor(int phID)
{	
	if (phID == 1 || phID == 2)
	{
		this->phID = phID;
		if (!initPh())
		{
			throw runtime_error(string("Kein Geraet gefunden!"));
		}
	}
	else
	{
		throw runtime_error(string("Zur Zeit sind nur PH-Modul 1 und 2 vorgesehen!"));
	}
}


phSensor::~phSensor()
{
}


bool phSensor::initPh()
{
	int address;
	bool res = true;
	
	if (phID == 1)
	{
		address = firstPHAddress;
	}
	else if (phID == 2)
	{
		address = secondPHAddress;
	}
	
	if (checkIfAddressIsUsed(address))
	{
		if (checkIfAddressIsUsed(factoryDefaultAddress))
		{
			res = false;
		}
		else
		{
			deviceId = initDevice(factoryDefaultAddress);
			if (!setNewBusAddress(address))
			{
				res = false;
			}
		}
	}
	else
	{
		deviceId = initDevice(address);	
		busAddress = address;
	}
	return res;
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
			
			deviceId = initDevice(newAddr);
			if (deviceId != -1)
			{				
				busAddress = newAddr;
				res = true;				
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


std::string phSensor::getDeviceModell()
{
	
	
}
