#pragma once

#include <vector>
#include <string>

//Adjuster ID
#define ADJUSTER_ONE 1
#define ADJUSTER_TWO 2

//Motor ID
#define PH_DOWN 1
#define PH_UP 2

//Adjuster Mode
#define FIXED 1
#define INTERVAL 2

//Motor shield I2C register
#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE
#define LED0_ON 0x6
#define LED0_OFF 0x8

#define LOW false
#define HIGH true

class phAdjuster
{
public:
	phAdjuster(int slotPosition);
	virtual ~phAdjuster();
	void initPHAdjuster();
	void setOperatingStatus(int adjusterID, bool active);
	bool getOperatingStatus(int adjusterID);
	void setTargetPHVal(float phVal, int adjusterID);
	float getTargetPHVal(int adjusterID);
	void assignToPHSlot(int slotPosition, int adjusterID);
	int getAssignedPHSlot(int adjusterID);
	void setMode(int adjusterID, int mode);
	int getMode(int adjusterID);
	bool checkConnection();
	int getSlotPosition();
	bool getPHDownInitStatus(int adjusterID);
	void setPHDownInitStatus(int adjusterID, bool status);
	bool getPHUpInitStatus(int adjusterID);
	void setPHUpInitStatus(int adjusterID, bool status);
	
	bool startPump(int adjusterID, int motorID, int speedVal);
	bool stopPump(int adjusterID, int motorID);

	void loadSettingsIfExist();
	void saveSettings();
	
private:
	void setPWM(int pin, int on, int off);
	void setSpeed(int speedVal);
	void setPin(int pin, bool val);
	void getMotor(int adjusterID, int motorID);
	void setPWMFreq();	
	std::string getFilepath();
	
	int PWMPin;
	int IN1Pin;
	int IN2Pin;
	int deviceID;
	
	bool adjOperating[2];
	bool phDownInitStatus[2];
	bool phUpInitStatus[2];
	int adjMode[2];
	float targetPHVal[2];
	int linkedPHSensorSlot[2];
	int slotPosition;
	
	bool disconnected;
};

extern std::vector<phAdjuster*> phAdjusters;

static int initPHAdjuster(int slotPosition)
{	
	phAdjusters.push_back(new phAdjuster(slotPosition));
	phAdjusters[phAdjusters.size() - 1]->loadSettingsIfExist();
	return (phAdjusters.size() - 1);
}