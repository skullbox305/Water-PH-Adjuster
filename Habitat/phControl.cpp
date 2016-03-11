#include "phControl.h"
#include "phSensor.h"
#include "phAdjuster.h"
#include "HabitatConfiguration.h"
#include "globalMtx.h"

#include <unistd.h>


void startPHControl()
{
	while (1)
	{
		
		if (!phSensors.empty())
		{
			adjustPH();
		}
	
		sleep(30);		
	}
}


void adjustPH()
{
	if (!phAdjusters.empty())
	{
		for (int phAdjObjID = 0; phAdjObjID < phAdjusters.size(); phAdjObjID++)
		{
			bool running = phAdjusters[phAdjObjID]->getOperatingStatus(ADJUSTER_ONE);
			int linkedSlot = phAdjusters[phAdjObjID]->getAssignedPHSlot(ADJUSTER_ONE);
			
			if (  running && (linkedSlot != -1) )
			{
				adjustPHOne(phAdjObjID);
			}			
		}
	}
}


void adjustPHOne(int phAdjObjectID)
{
	int phSensorSlot = phAdjusters[phAdjObjectID]->getAssignedPHSlot(ADJUSTER_ONE);
	int mode = phAdjusters[phAdjObjectID]->getMode(ADJUSTER_ONE);
	float targetPH;
	float currentPH;
	bool phDisconnected;
	bool phSensorActive;
	
	if (mode == INTERVAL)
	{
		
	}
	
	targetPH = phAdjusters[phAdjObjectID]->getTargetPHVal(ADJUSTER_ONE);
		
	if (deviceObjectIdList[phSensorSlot - 1] != -1)
	{
		phDisconnected = phSensors[deviceObjectIdList[phSensorSlot - 1]]->isDisconnected();
		phSensorActive = phSensors[deviceObjectIdList[phSensorSlot - 1]]->isOperating();
				
		if (!phDisconnected && phSensorActive)
		{
			currentPH = phSensors[deviceObjectIdList[phSensorSlot - 1]]->getNewPHReading();
			
			if ((currentPH >= (targetPH + 0.05)))
			{
				adjustPHDown(phAdjObjectID, ADJUSTER_ONE);
			}
			else if (currentPH <= (targetPH - 0.05))
			{
				adjustPHUp(phAdjObjectID, ADJUSTER_ONE);
			}
		}
	}
}


void adjustPHTwo(int phAdjObjectID)
{
}



void adjustPHDown(int phAdjObjectID, int adjusterID)
{
	int phSensorSlot = phAdjusters[phAdjObjectID]->getAssignedPHSlot(adjusterID);
	float targetPHValue = phAdjusters[phAdjObjectID]->getTargetPHVal(adjusterID);
	float currentPhValue = phSensors[deviceObjectIdList[phSensorSlot - 1]]->getPHReading();
	bool phDisconnected;
	bool phSensorActive;

	char buff[10];
	snprintf(buff, sizeof(buff), "%.2f", currentPhValue);
	
	phAdjusters[phAdjObjectID]->startPump(adjusterID, PH_DOWN, 260);
	std::cout << "Slot " << phAdjusters[phAdjObjectID]->getSlotPosition() << ": DOWN - Target PH: " << targetPHValue << "  Current PH: " << buff << std::endl;
	
	do
	{    
		currentPhValue = phSensors[deviceObjectIdList[phSensorSlot - 1]]->getNewPHReading();
		phDisconnected = phSensors[deviceObjectIdList[phSensorSlot - 1]]->isDisconnected();
		phSensorActive = phSensors[deviceObjectIdList[phSensorSlot - 1]]->isOperating();
		snprintf(buff, sizeof(buff), "%.2f", currentPhValue);
		std::cout << "Slot " << phAdjusters[phAdjObjectID]->getSlotPosition() << ": DOWN - Target PH: " << targetPHValue << "  Current PH: " << buff << std::endl;
	} while ((currentPhValue > (targetPHValue + 0.08)) && !phDisconnected && phSensorActive);
	
	phAdjusters[phAdjObjectID]->stopPump(adjusterID, PH_DOWN);
}


void adjustPHUp(int phAdjObjectID, int adjusterID)
{
	int phSensorSlot = phAdjusters[phAdjObjectID]->getAssignedPHSlot(adjusterID);
	float targetPHValue = phAdjusters[phAdjObjectID]->getTargetPHVal(adjusterID);
	float currentPhValue = phSensors[deviceObjectIdList[phSensorSlot - 1]]->getPHReading();
	bool phDisconnected;
	bool phSensorActive;
	
	char buff[10];
	snprintf(buff, sizeof(buff), "%.2f", currentPhValue);
	
	phAdjusters[phAdjObjectID]->startPump(adjusterID, PH_UP, 260);
	std::cout << "Slot " << phAdjusters[phAdjObjectID]->getSlotPosition() << ": UP - Target PH: " << targetPHValue << "  Current PH: " << buff << std::endl;
	
	do
	{    
		currentPhValue = phSensors[deviceObjectIdList[phSensorSlot - 1]]->getNewPHReading();
		phDisconnected = phSensors[deviceObjectIdList[phSensorSlot - 1]]->isDisconnected();
		phSensorActive = phSensors[deviceObjectIdList[phSensorSlot - 1]]->isOperating();	
		snprintf(buff, sizeof(buff), "%.2f", currentPhValue);
		std::cout << "Slot " << phAdjusters[phAdjObjectID]->getSlotPosition() << ": UP - Target PH: " << targetPHValue << "  Current PH: " << buff << std::endl;
	} while ((currentPhValue < (targetPHValue - 0.08)) && !phDisconnected && phSensorActive);
	
	phAdjusters[phAdjObjectID]->stopPump(adjusterID, PH_UP);
}


//void getCurrentTargetPH(int phAdjObjectID, int adjusterID)
//{
//	time_t currentDate = time(NULL);
//	time_t dateNextTick = phAdjusters[phAdjObjectID]->getDateForNextTick();
//	
//	if (difftime(currentDate, dateNextTick) > 0) 
//	{
//		if
//	}
//	
//
//	
//}
//