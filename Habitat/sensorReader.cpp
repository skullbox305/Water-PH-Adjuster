#include "sensorReader.h"
#include "phSensor.h"
#include "databaseOperations.h"

#include <unistd.h>
#include <iostream>

using namespace std;


void startSensorReader()
{	
	while (1)
	{
		readPH();
		sleep(20);
	}
}


void readPH()
{
	if (!phSensors.empty()) 
	{
		for (int i = 0; i < phSensors.size(); i++)
		{
			char buff[50];
			snprintf(buff, sizeof(buff), "%.2f", phSensors[i]->getNewPHReading());
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


