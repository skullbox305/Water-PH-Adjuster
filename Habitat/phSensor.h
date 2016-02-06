#pragma once

#include <iostream>
#include <string>
#include <vector>

class phSensor
{
public:
	phSensor();
	phSensor(int busAddr);
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
};

extern std::vector<phSensor*> phSensors; //während programm läuft immer aktiv. Beim beendet gibt OS speicher eh frei. Delete notwendig?
extern const int maxAmountPHModules;
extern const int factoryDefaultAddress;
extern const int startPHAddress;
extern bool slotsUsed[];

static void initPhModules()
{
	int address = startPHAddress;
	
	for (int i = 0; i++; i < maxAmountPHModules)
	{
		if (slotsUsed[i] == true)
		{
			phSensors.push_back(new phSensor(address));
		}
		address += 0x1;
	}	
};
