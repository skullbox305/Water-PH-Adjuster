#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>

class phSensor
{
public:
	phSensor();
	phSensor(int busAddr, int position);
	~phSensor();
	float getNewPHReading();
	float getPHReading();
	int getBusAddress();
	float getTempCompensation();
	bool setTempCompensation(float newTemp);
	int getCalibrationStatus();
	bool clearCalibration();
	bool lowpointCalibration(float phVal);
	bool midpointCalibration(float phVal);
	bool highpointCalibration(float phVal);
	bool startSleepmode();
	std::string getDeviceInfo();
	bool getSlope(float &acidCalibration, float &baseCalibration); //fehlerhaft. manchal z.B.90% aber auch 100% möglich
	bool setNewBusAddress(int newAddr);	
	bool checkDeviceModell();
	
private:
	bool calibration(std::string cmd, float phVal);
	
	float phValue;
	int deviceID;
	int busAddress;
	int slotPosition;
	bool disconnected;
	std::mutex phMtx;
};

extern std::vector<phSensor*> phSensors; //während programm läuft immer aktiv. Beim beendet gibt OS speicher eh frei. Delete notwendig?



////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Initialises the ph module.</summary>
///
/// <param name="addr">		   The i2c address</param>
/// <param name="slotPosition">The slot position in the water sensor array (1-16)</param>
////////////////////////////////////////////////////////////////////////////////////////////////////
static void initPhModule(int addr, int slotPosition)
{	
	phSensors.push_back(new phSensor(addr, slotPosition));		
}

