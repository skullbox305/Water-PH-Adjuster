#include "phAdjuster.h"
#include "sensorReader.h"
#include "HabitatConfiguration.h"

#include <thread>
#include <mutex>

using namespace std;

mutex sensorMtx;

void SensorReaderThread()
{
	startSensorReader(ref(sensorMtx));
}

void phAdjusterThread()
{	
}
 
int main()
{	
	loadHabitatConfiguration();
	//thread sensorReader(sensorReaderThread);
	//thread phController(phAdjusterThread);
	
	//sensorReader.join();
	//phController.join();
		
	return 0;	
}