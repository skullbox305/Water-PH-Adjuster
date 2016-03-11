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
		sleep(5);
	}
}


void readPH()
{
	if (!phSensors.empty()) 
	{
		for (int i = 0; i < phSensors.size(); i++)
		{
			char buff[50];
			if (phSensors[i]->isOperating())
			{
				snprintf(buff, sizeof(buff), "%.2f", phSensors[i]->getNewPHReading());
			
				//phSensors[i]->startSleepmode(); //dont overdo it. It seems like the device tend to freeze at some point
				string phStr = buff;
				cout << "Slot " << phSensors[i]->getSlotPosition() << ": " << phStr << endl;
			}
			else
			{
				cout << "PH-Reading paused!" << endl;
			}
		}
	}
	else
	{
		cout << "kein ph" << endl;
	}
}


