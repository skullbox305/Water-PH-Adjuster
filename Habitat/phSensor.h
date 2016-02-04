#pragma once
#include <iostream>
#include <string>

class phSensor
{
public:
	phSensor(int busAddr);
	virtual ~phSensor();
	float getNewPHReading();
	float getLastPHReading();
	int getBusAddress();
	bool setNewBusAddress(int newAddr);
	float getTempCompensation();
	bool setTempCompensation(float newTemp);
	int getCalibrationStatus();
	bool clearCalibration();
	bool lowpointCalibration(float phVal);
	bool midpointCalibration(float phVal);
	bool highpointCalibration(float phVal);
	bool startSleepmode();
	bool getDeviceInfo(std::string &info);
	bool getSlope(float &acidCalibration, float &baseCalibration);
	
	std::string getDeviceModell();
	
	
private:
	bool calibration(std::string cmd, float phVal);
	float lastPHValue;
	int deviceId;
	int busAddress;
};
