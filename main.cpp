#include "phAdjuster.h"

#include "sensorReader.h"
#include "phControl.h"

#include "HabitatConfiguration.h"
#include "wiringPiI2C.h"

#include <thread>

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

	addNewDevice(1, PH);
	addNewDevice(2, PH_ADJUSTER);
		
	phSensors[0]->setOperatingStatus(true);
	phSensors[0]->setTempCompensation(20.0);
	phAdjusters[0]->assignToPHSlot(1, ADJUSTER_ONE);
	phAdjusters[0]->setOperatingStatus(ADJUSTER_ONE, true);
	phAdjusters[0]->setTargetPHVal(6.0, ADJUSTER_ONE);
		
	thread sensorreader(sensorReaderThread);
	thread phcontroller(phAdjusterThread);

	sensorreader.join();
	phcontroller.join();

	return 0;	
}