#pragma once

#include <iostream>
#include <string>
#include <vector>

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
	bool getSlope(float &acidCalibration, float &baseCalibration);
	bool setNewBusAddress(int newAddr);	
	void syncSharedMemory();
	bool checkDeviceModell();
	
private:
	bool calibration(std::string cmd, float phVal);
	
	//shared-variable
	float phValueWrite; ///< The ph value write segment
	float phValueRead;  ///< The ph value read segment. Mutex getPHValue() for max performance
	
	int deviceId;
	int busAddress;
	int slotPosition;
};

extern std::vector<phSensor*> phSensors; //während programm läuft immer aktiv. Beim beendet gibt OS speicher eh frei. Delete notwendig?
extern const int factoryDefaultPHAddress;

static void initPhModule(int addr, int slotPosition)
{	
	phSensors.push_back(new phSensor(addr, slotPosition));		
}

