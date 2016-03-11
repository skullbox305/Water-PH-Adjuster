/**
 * @file phSensor.cpp
 *
 * Implements the ph sensor class.
 */

#include "phSensor.h"
#include "i2c_iO.h"
#include "globalMtx.h"

#include <iostream>
#include <unistd.h>
#include <string>
#include <stdexcept>
#include <wiringPiI2C.h>

#define DEFAULT_ADDR 0x63

using namespace std;

vector<phSensor*> phSensors;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Constructor.</summary>
///
/// <exception cref="runtime_error">If no device is found or the set phID is != (1 or 2)</exception>
///
/// <param name="phID">ID of the PH Slot (1 or 2 for the moment)</param>
////////////////////////////////////////////////////////////////////////////////////////////////////
phSensor::phSensor(int position)
{	
	busMtx.lock();
	slotPosition = position;
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	busMtx.unlock();
	
	if (deviceID < 0)
	{
		throw runtime_error(string("error while opening ph on default address 0x63"));
	}
	busAddress = DEFAULT_ADDR;	
	phValue = 0;
	disconnected = false;
	running = true;
}



/// <summary>Destructor.</summary>
phSensor::~phSensor()
{
}



////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Gets the current bus address.</summary>
///
/// <returns>The current bus address.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
int phSensor::getBusAddress()
{
	return busAddress;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Sets a new bus address in the firmware of the device.</summary>
///
/// <param name="newAddr">The new bus address.</param>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool phSensor::setNewBusAddress(int newAddr)
{
	bool res = false;
	
	if (newAddr > 0 && newAddr < 128)
	{
		char buffer[10];
		sprintf(buffer, "I2c,%d", newAddr);
		
		busMtx.lock();
		deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
		res = writeI2CAtlasScientific(string(buffer), deviceID);
	
		if (res)
		{		
			sleep(3);
			
			deviceID = wiringPiI2CSetup(newAddr);
			
			if (deviceID != -1)
			{				
				busAddress = newAddr;
				res = true;				
			}
			else
			{
				res = false;
			}
		}
	}	
	busMtx.unlock();
	return res;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Gets current temperature compensation value.</summary>
///
/// <returns>The temperature compensation value.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
float phSensor::getTempCompensation()
{
	float tempCompensation = -1;
	bool res;
	
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	res = writeI2CAtlasScientific("T,?", deviceID);
	
	if (res)
	{
		string result;
		usleep(300000); //wait 300 microseconds for response
		
		res = readI2CAtlasScientific(result, deviceID);
		busMtx.unlock();
		
		if (res)
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


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Sets temperature compensation value. In order to achieve the most accurate possible
/// 	readings, the temperature of the liquid being measured must be transmitted to the ph-
/// 	module. Another device must be used to read the temperature.</summary>
///
/// <param name="newTemp">The new temperature compensation value.</param>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool phSensor::setTempCompensation(float newTemp)
{
	bool res = false;
	char buffer[10];
	sprintf(buffer, "T,%f", newTemp);
	
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	res = writeI2CAtlasScientific(string(buffer), deviceID);
	
	if (res)
	{
		usleep(300000); //wait 300 microseconds for response

		string reading;
		
		res = readI2CAtlasScientific(reading, deviceID);
	}
	busMtx.unlock();
	return res;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Gets calibration status.</summary>
///
/// <returns>"-1" == error
/// 	 "0"	== no calibration profile present "1" == one point calibrated "2" == two point
/// 	 calibrated "3" == three point calibrated.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
int phSensor::getCalibrationStatus()
{
	int result = -1;
	bool res;
	
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	res = writeI2CAtlasScientific("Cal,?", deviceID);
	
	if (res)
	{
		usleep(300000);

		string reading;
		
		res = readI2CAtlasScientific(reading, deviceID);
		
		if (res)
		{
			if (isdigit(reading[5]))
			{
				result = reading[5] - '0';
			}
		}
	}
	busMtx.unlock();
	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Clears the calibration profile.</summary>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool phSensor::clearCalibration()
{
	bool res = false;
	
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	res = writeI2CAtlasScientific("Cal,clear", deviceID);
	
	if (res)
	{
		usleep(300000); //wait 300ms for response
		string reading;
		
		res = readI2CAtlasScientific(reading, deviceID);
	}
	busMtx.unlock();
	return res;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Calibrations of the ph-module.</summary>
///
/// <param name="cmd">  The command which gets send to the device.</param>
/// <param name="phVal">The ph value of the calibration solution.</param>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool phSensor::calibration(std::string cmd, float phVal)
{
	bool res = false;
	char buffer[12];
	sprintf(buffer, "%s,%f", cmd.c_str(), phVal);
	
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	res = writeI2CAtlasScientific(string(buffer), deviceID);
	
	if (res)
	{
		disconnected = false;
		usleep(1600000);//wait 1.6 sec for response
		string reading;
		
		res = readI2CAtlasScientific(reading, deviceID);
	}
	else
	{
		disconnected = true;
	}
	busMtx.unlock();
	return res;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Lowpoint calibration i.e. 4.0.</summary>
///
/// <param name="phVal">The ph value of the calibration solution.</param>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool phSensor::lowpointCalibration(float phVal)
{
	return calibration("Cal,low", phVal);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Midpoint calibration (7.0 recommended).</summary>
///
/// <param name="phVal">The ph value of the calibration solution.</param>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool phSensor::midpointCalibration(float phVal)
{
	return calibration("Cal,mid", phVal);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Highpoint calibration i.e. 10.0.</summary>
///
/// <param name="phVal">The ph value of the calibration solution.</param>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool phSensor::highpointCalibration(float phVal)
{
	return calibration("Cal,high", phVal);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 		 Gets a new ph reading from the probe. Use this function just in one thread if possible.
/// 		 A Mutex would take at least 1 second, so poor performance.
/// </summary>
///
/// <returns>  ">= 0" -> ph-value, "-1"   -> reading failed. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
float phSensor::getNewPHReading()
{
	busMtx.lock();
	
	bool res;
	
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	res = writeI2CAtlasScientific("R", deviceID);
	
	if (res)
	{
		disconnected = false;
		sleep(1); //wait 1 sec for response
		string result;
		
		res = readI2CAtlasScientific(result, deviceID);
		
		if (res)
		{
			if (sscanf(result.c_str(), "%f", &phValue) != 1) 
			{
				phValue = -1;
			}
		}
		else
		{
			disconnected = true;
		}
	}
	else
	{
		disconnected = true;
		phValue = -1;
	}
	busMtx.unlock();
	
	return phValue;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Starts the sleepmode of the ph-module.
/// 	
/// 	To conserve energy in between readings, the ph-module can be put into a low power sleep
/// 	state. This will turn off the LEDs and shut down almost all of the internal workings. The
/// 	power consumption will be reduced to 1.16 mA at 5V and 0.995 mA at 3.3V. To wake the ph-
/// 	module, send it any command. After the device is woken up, 4 consecutive readings should
/// 	be taken before the readings are considered valid.</summary>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool phSensor::startSleepmode()
{
	bool res;
	
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	res = writeI2CAtlasScientific("SLEEP", deviceID);
	
	if (res)
	{
		disconnected = false;
		usleep(300000); //wait 300ms till next instruction can be recieved
	}
	else
	{
		disconnected = true;
	}
	busMtx.unlock();
	return res;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Gets the device information.</summary>
///
/// <returns>
/// 	- Device info on success  
/// 	- Empty String (use empty() function)
///	</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
string phSensor::getDeviceInfo()
{
	char buffer[30];
	string result = "";
	bool res;
	
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	res = writeI2CAtlasScientific("I", deviceID);
	
	if (res)
	{
		usleep(300000); //wait 300ms for instruction
		
		res = readI2CAtlasScientific(result, deviceID);

		if (res)
		{
			disconnected = false;
			sprintf(buffer, "%s%s%s%s", "Device: ", result.substr(3, 2).c_str(), "  Firmware: ", result.substr(6, 3).c_str());
			result = string(buffer);
		}
		else
		{
			disconnected = true;
			result = "";
		}
	}
	else
	{
		disconnected = true;
	}
	busMtx.unlock();
	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Gets the slope. After calibrating a pH probe issuing the slope command will show how
/// 	closely (in percentage) the calibrated pH probe is working compared to an “ideal” pH
/// 	probe.</summary>
///
/// <param name="acidCalibration">[in,out] Float in which the result of the acid calibration slope
/// 	will be stored.</param>
/// <param name="baseCalibration">[in,out] Float in which the result of the base calibration slope
/// 	will be stored.</param>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool phSensor::getSlope(float &acidCalibration, float &baseCalibration)
{
	bool res;
	
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	res = writeI2CAtlasScientific("SLOPE,?", deviceID);
	
	if (res)
	{
		usleep(300000); //wait 300ms for instruction
		string result;
		
		res = readI2CAtlasScientific(result, deviceID);
		
		if (res)
		{
			string acid = result.substr(7, 4).c_str();
			string base = result.substr(12, 4).c_str();
			if (sscanf(acid.c_str(), "%f", &acidCalibration) && sscanf(base.c_str(), "%f", &baseCalibration) != 1)
			{
				disconnected = false;
				res = false;
			}
			else
			{
				disconnected = true;
			}		
		}
	}
	else
	{
		disconnected = false;
	}
	busMtx.unlock();
	
	return res;	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 		 Gets the last PH reading gotten by getNewPHReading().
/// </summary>
///
/// <returns>The current ph-value</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
float phSensor::getPHReading()
{
	float ph;

	busMtx.lock();
	ph = phValue;
	busMtx.unlock();
	
	return phValue;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Determines if the device is a ph-module.</summary>
///
/// <returns>true if it is a ph, false if it is not.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool phSensor::checkDeviceModell()
{
	bool res = false;
	char buffer[30];
	
	busMtx.lock();
	deviceID = switchToSlot(slotPosition, DEFAULT_ADDR);
	res = writeI2CAtlasScientific("I", deviceID);
	
	if (res)
	{
		usleep(300000); //wait 300ms for instruction
		string result;
		
		res = readI2CAtlasScientific(result, deviceID);
		
		if (res)
		{
			sprintf(buffer, "%s", result.substr(3, 2).c_str());
			result = string(buffer);
			if (result != "pH")
			{
				disconnected = false;
				res = false;
			}
			else
			{
				disconnected = true;
			}
		} 		
	}
	busMtx.unlock();
	
	return res;	
}


bool phSensor::isDisconnected()
{
	return disconnected;
}


bool phSensor::isOperating()
{
	return running;
}


void phSensor::setOperatingStatus(bool running)
{
	this->running = running;
}


int phSensor::getSlotPosition()
{
	return slotPosition;
}
