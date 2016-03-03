#pragma once

#include <vector>

//Motor ID
#define PH_UP 1
#define PH_DOWN 2

//Adjuster ID
#define ADJUSTER_ONE 1
#define ADJUSTER_TWO 2

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
	void assignPHSlot(int slotPosition, int adjusterID);
	int getAssignedPHSlot(int adjusterID);
	
	bool startPump(int adjusterID, int motorID, int speedVal);
	bool stopPump(int adjusterID, int motorID);
	
	void loadPreviousConfig();
	
private:
	void getPhObjectID();
	void setPWM(int pin, int on, int off);
	void setSpeed(int speedVal);
	void setPin(int pin, bool val);
	void getMotor(int adjusterID, int motorID);
	void setPWMFreq();	
	
	int PWMPin;
	int IN1Pin;
	int IN2Pin;
	int deviceID;
	int phObjVectorID[2];
	
	bool adjActive[2];
	int adjMode[2];
	float targetPHVal[2];
	int linkedPHSensorSlot[2];
	int slotPosition;
	bool disconnected;
};

extern std::vector<phAdjuster*> phAdjusters;

static void initPHAdjuster(int slotPosition)
{	
	phAdjusters.push_back(new phAdjuster(slotPosition));
}