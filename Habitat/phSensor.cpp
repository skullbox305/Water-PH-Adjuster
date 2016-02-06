/**
 * @file phSensor.cpp
 *
 * Implements the ph sensor class.
 */

#include "phSensor.h"
#include "AtlasScientific_i2c_iO.h"

#include <iostream>
#include <unistd.h>
#include <string>
#include <stdexcept>

using namespace std;

vector<phSensor*> phSensors;
const int maxAmountPHModules = 2;
const int factoryDefaultAddress = 0x63;
const int startPHAddress = 0x3;
bool slotsUsed[maxAmountPHModules] = { false };


/// <summary>Default constructor.</summary>
phSensor::phSensor()
{
	deviceId = initDevice(factoryDefaultAddress);
	if (deviceId < 0)
	{
		throw runtime_error(string("error while opening ph on defaul address 0x63"));
	}
	busAddress = factoryDefaultAddress;	
}



////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Constructor.</summary>
///
/// <exception cref="runtime_error">If no device is found or the set phID is != (1 or 2)</exception>
///
/// <param name="phID">ID of the PH Slot (1 or 2 for the moment)</param>
////////////////////////////////////////////////////////////////////////////////////////////////////
phSensor::phSensor(int busAddr)
{	
	deviceId = initDevice(busAddr);
	if (deviceId < 0)
	{
		char buffer[10];
		sprintf(buffer, "%x", busAddr);
		throw runtime_error(string("error while opening ph on address 0x" + string(buffer)));
	}
	busAddr = factoryDefaultAddress;	
}



/// <summary>Destructor.</summary>
phSensor::~phSensor()
{
	cout << "destructed" << endl;
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


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Gets current temperature compensation value.</summary>
///
/// <returns>The temperature compensation value.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Clears the calibration profile.</summary>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
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
	if (writeI2C("R", deviceId))
	{
		sleep(1); //wait 1 sec for response

		string result;
		if (readI2C(result, deviceId))
		{
			if (sscanf(result.c_str(), "%f", &phValueWrite) != 1) 
			{
				cout << "Bad reading: " << result << endl;
				phValueWrite = -1;
			}
		} 		
	}
	return phValueWrite;
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
	bool res = false;
	if (writeI2C("SLEEP", deviceId))
	{
		usleep(300000); //wait 300ms for instruction
		res = true;
	}
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
	
	if (writeI2C("I", deviceId))
	{
		usleep(300000); //wait 300ms for instruction

		if (readI2C(result, deviceId))
		{
			sprintf(buffer, "%s%s%s%s", "Device: ", result.substr(3, 2).c_str(), "  Firmware: ", result.substr(6, 3).c_str());
			result = string(buffer);
		}
		else
		{
			result = "";
		}
	}
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


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 		 Gets the last PH reading gotten by getNewPHReading().
/// </summary>
///
/// <returns>The ph-value</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
float phSensor::getPHReading()
{
	return phValueRead;
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
	
	if (writeI2C("I", deviceId))
	{
		usleep(300000); //wait 300ms for instruction

		string result;
		if (readI2C(result, deviceId))
		{
			sprintf(buffer, "%s", result.substr(3, 2).c_str());
			result = string(buffer);
			if (result == "pH")
			{
				res = true;
			}
		} 		
	}
	return res;	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///		Copies the current ph-value from the write segment of the shared-variable to read segment. 
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
void phSensor::syncSharedMemory()
{
	phValueRead = phValueWrite;
}
