#include "phControl.h"
#include "phSensor.h"
#include "phAdjuster.h"
#include "HabitatConfiguration.h"
#include "globalMtx.h"

#include <unistd.h>



void initPHAdjusters()
{
	if (!phAdjusters.empty())
	{
		for (int i = 0; i < phAdjusters.size(); i++)
		{
			
		}
	}
}


void startPHControl()
{
	while (1)
	{
		adjustPH();
		sleep(10);		
	}
}


void adjustPH()
{
	if (!phAdjusters.empty())
	{
		for (int i = 0; i < phAdjusters.size(); i++)
		{
			fillTubesIfNot(i);
		}
	}
}


void fillTubesIfNot(int phAdjObjectID)
{
	fillTubes(phAdjObjectID, ADJUSTER_ONE);
	//fillTubes(phAdjObjectID, ADJUSTER_TWO);
}


void fillTubes(int phAdjObjectID, int adjusterID)
{
	if (phAdjusters[phAdjObjectID]->getOperatingStatus(adjusterID) == true)
	{
		if (phAdjusters[phAdjObjectID]->getPHDownInitStatus(adjusterID) == false)
		{
			initPHDown(phAdjObjectID, adjusterID);
		}
		
		if (phAdjusters[phAdjObjectID]->getPHUpInitStatus(adjusterID) == false)
		{
			initPHUp(phAdjObjectID, adjusterID);
		}		
	}
}


void initPHDown(int phAdjObjectID, int adjusterID)
{
	int phSensorSlot = phAdjusters[phAdjObjectID]->getAssignedPHSlot(adjusterID);
	float phStartValue;
	float currentPhValue;
	
	phReadMtx.lock();
	phStartValue = phSensors[deviceObjectIdList[phSensorSlot]]->getNewPHReading();
	phReadMtx.unlock();
	
	phAdjusters[phAdjObjectID]->startPump(adjusterID, PH_DOWN, 255);
	
	do
	{    
		phReadMtx.lock();
		currentPhValue = phSensors[deviceObjectIdList[phSensorSlot]]->getNewPHReading();
		std::cout << "Down - Current PH: " << currentPhValue << std::endl;
		phReadMtx.unlock();
	} while ( !(currentPhValue < (phStartValue - 0.05)) );
	
	phAdjusters[phAdjObjectID]->stopPump(adjusterID, PH_DOWN);
	phAdjusters[phAdjObjectID]->setPHDownInitStatus(adjusterID, true);	
}


void initPHUp(int phAdjObjectID, int adjusterID)
{
	int phSensorSlot = phAdjusters[phAdjObjectID]->getAssignedPHSlot(adjusterID);
	float phStartValue;
	float currentPhValue;
	
	phReadMtx.lock();
	phStartValue = phSensors[deviceObjectIdList[phSensorSlot]]->getNewPHReading();
	phReadMtx.unlock();
	
	phAdjusters[phAdjObjectID]->startPump(adjusterID, PH_UP, 255);
	
	do
	{    
		phReadMtx.lock();
		currentPhValue = phSensors[deviceObjectIdList[phSensorSlot]]->getNewPHReading();
		std::cout << "UP - Current PH: " << currentPhValue << std::endl;
		phReadMtx.unlock();
	} while (!(currentPhValue > (phStartValue + 0.05)));
	
	phAdjusters[phAdjObjectID]->stopPump(adjusterID, PH_UP);
	phAdjusters[phAdjObjectID]->setPHUpInitStatus(adjusterID, true);
}
