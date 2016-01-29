#include "phSensor.h"
#include "phAdjuster.h"

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define function 1

using namespace std;
 
int main()
{
	phSensor ph1(0x63);
	string temp;
	float acidCalibration;
	float baseCalibration;	
	
	switch (function)
	{
	case 1: 
		cout << ph1.getNewPHReading() << endl;
		cout << ph1.getLastPHReading() << endl;
		break;		
	case 2:
		cout << ph1.getCalibrationStatus() << endl;
		break;
	case 3:
		cout << ph1.getTempCompensation() << endl;
		break;
	case 4:
		cout << ph1.clearCalibration() << endl;
		break;
	case 5:
		cout << ph1.setTempCompensation(21) << endl;
		break;
	case 6:
		cout << ph1.lowpointCalibration(3) << endl;
		break;
	case 7:
		cout << ph1.midpointCalibration(7) << endl;
		break;
	case 8:
		cout << ph1.highpointCalibration(10) << endl;
		break;
	case 9:
		cout << ph1.setNewBusAddress(0x64) << endl;
		cout << ph1.getNewPHReading() << endl;
		break;
	case 10:
		cout << ph1.setNewBusAddress(0x63) << endl;
		cout << ph1.startSleepmode() << endl;
		break;
	case 11:
		cout << ph1.getDeviceInfo(temp) << endl;
		cout << temp << endl;
		break;
	case 12:
		cout << ph1.getSlope(acidCalibration, baseCalibration) << endl;
		cout << "Acid calibration: " << acidCalibration << endl;
		cout << "Base calibration: " << baseCalibration << endl;
		break;
	default:
		break;
	}
	return 0;	
}