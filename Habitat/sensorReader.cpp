#include "sensorReader.h"
#include "phSensor.h"
#include "databaseOperations.h"

#include <unistd.h>
#include <iostream>

using namespace std;

mutex *sensorMtx;


void initSensorReader()
{
	initPhModules();
}


void startSensorReader(mutex *mtx)
{
	initSensorReader();
	sensorMtx = mtx;
	
	while (1)
	{
		readSensors();
		sleep(5);
	}
}


void readPH()
{
	if (!phSensors.empty())
	{
		for (int i = 0; i++; i < phSensors.size())
		{
			cout << phSensors[i]->getNewPHReading() << endl;
			sensorMtx->lock();
			phSensors[i]->syncSharedMemory();
			sensorMtx->unlock();
		}
	}
}


void readSensors()
{
	readPH();
}
