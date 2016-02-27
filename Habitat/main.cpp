#include "phAdjuster.h"
#include "sensorReader.h"
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
	
}

 
int main()
{	
	//loadHabitatConfiguration();
	//addNewCircuit(1, PH);
	//thread sensorreader(sensorReaderThread);
	//thread phcontroller(phAdjusterThread);
	
	//sensorreader.join();
	//phcontroller.join();
	
	initPHAdjuster(5);
	phAdjusters[0]->startPump(adjusterOne, phUP , 255);
	sleep(1);
 	phAdjusters[0]->stopPump(adjusterOne, phUP);
	
	
	
	
	
	
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