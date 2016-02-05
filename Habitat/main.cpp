#include "phSensor.h"
#include "phAdjuster.h"
#include "AtlasScientific_i2c_iO.h"

#include <iostream>
#include <string.h>

#define function 14

using namespace std;
 
int main()
{
	phSensor ph1(1);
	string temp;
	float acidCalibration;
	float baseCalibration;	
	
	switch (function)
	{
	case 1: 
		for (int i = 0; i < 10; i++)
		{
			cout << ph1.getNewPHReading() << endl;
			ph1.startSleepmode();
		}		
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

		break;
	case 10:
		
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
	case 13:
		cout << checkIfAddressIsUsed(0x64) << endl;
		break;
	case 14:
		cout << "Address: " << ph1.getBusAddress() << endl;
		cout << ph1.getDeviceInfo(temp) << endl;
		cout << temp << endl;
		cout << "PH Wert: " << ph1.getNewPHReading() << endl;
		cout << ph1.startSleepmode() << endl;
		cout << ph1.setNewBusAddress(0x63) << endl;
		cout << "Address: " << ph1.getBusAddress() << endl;

	default:
		break;
	}
	return 0;	
}