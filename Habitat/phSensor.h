#pragma once
#include <iostream>
#include <string>

class phSensor
{
public:
	phSensor(int phID);
	~phSensor();
	float getNewPHReading();
	float getLastPHReading();
	int getBusAddress();
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
	bool setNewBusAddress(int newAddr);
	
	
private:
	bool calibration(std::string cmd, float phVal);
	bool initPh();
	std::string getDeviceModell();
	float lastPHValue = 0;
	int deviceId = 0;
	int busAddress = 0;
	int phID;
};
