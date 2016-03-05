#include "phAdjuster.h"

#include "sensorReader.h"
#include "phControl.h"

#include "HabitatConfiguration.h"

#include <thread>
#include <vector>
#include <wiringPiI2C.h>
#include <unistd.h>

using namespace std;

void sensorReaderThread()
{
	startSensorReader();
}

void phAdjusterThread()
{
	startPHControl();
}

 
int main()
{	
	loadHabitatConfiguration();
	
//	phAdjusters[0]->setPHDownInitStatus(ADJUSTER_ONE, false);
//	phAdjusters[0]->setPHUpInitStatus(ADJUSTER_ONE, false);
		
	
		
	thread sensorreader(sensorReaderThread);
	thread phcontroller(phAdjusterThread);

	sensorreader.join();
	phcontroller.join();
	

//	addNewDevice(1, PH);
//	addNewDevice(2, PH_ADJUSTER);
//	loadHabitatConfiguration();	
	
	
	
	
	//vector<float> avg;
	//phSensor ph(0x3, 0);
	//cout << ph.lowpointCalibration(3.04) << endl;
	//
	//for (int i = 0; i < 20; i++)
	//{
		//avg.push_back(ph.getNewPHReading());
	//}
	//float sum = 0;
	//for (float i : avg)
	//{
		//sum += i;
	//}
	//
	//cout << sum / avg.size() << endl;
	return 0;	
}