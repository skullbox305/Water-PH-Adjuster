#include "sensorReader.h"
#include "phSensor.h"
#include "globalMtx.h"

#include <unistd.h>
#include <iostream>

using namespace std;


void startSensorReader()
{	
	while (1)
	{
		readPH();
		sleep(10);
	}
}


void readPH()
{
	if (!phSensors.empty()) 
	{
		for (int i = 0; i < phSensors.size(); i++)
		{
			char buff[50];
			phReadMtx.lock();
			snprintf(buff, sizeof(buff), "%.2f", phSensors[i]->getNewPHReading());
			phReadMtx.unlock();
			
			//phSensors[i]->startSleepmode();
			string phStr = buff;
			cout << phStr << endl;
		}
	}
	else
	{
		cout << "kein ph" << endl;
	}
}


