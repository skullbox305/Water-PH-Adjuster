#include "HabitatConfiguration.h"
#include "AtlasScientific_i2c_iO.h"
#include "phSensor.h"
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <array>

using namespace std;

const int maxWaterSensorAmount = 16;
const int waterSensorAddressSpace[] = { 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19 };
array<string, 16> deviceList;
int currentWaterSensorAmount;

void loadHabitatConfiguration()
{	
	deviceList.fill("");
	currentWaterSensorAmount = 0;
	checkAvailPHModules();
	checkAvailModules();
}


void reloadHabitatConfiguration()
{
	if (clearSensorObjects() != currentWaterSensorAmount)
	{
		perror("Es wurden nicht alle Objekte geloescht!");
	}
	deviceList.fill("");
	currentWaterSensorAmount = 0;
	checkAvailModules();
}

void checkAvailModules()
{
	checkAvailPHModules();
}

void checkAvailPHModules()
{	
	for (int i = 0; i < maxWaterSensorAmount; i++)
	{
		int address = waterSensorAddressSpace[i];
		if (checkIfAddressIsUsed(address))
		{
			phSensor ph(address, 0);
			if (ph.checkDeviceModell() == true)
			{
				deviceList[i] = "PH";
				initPhModule(address, i);
				currentWaterSensorAmount++;
			}
		}
	}
}


void addNewPHCircuit(int slotPosition)
{
	if (checkIfAddressIsUsed(factoryDefaultPHAddress))
	{
		if (checkIfAddressIsUsed(slotPosition))
		{
			if (changeFactoryAddress(slotPosition) == true)
			{
				reloadHabitatConfiguration();
			}
			else
			{
				perror("changing ph factory address 0x63 failed");
			}
		}
		else
		{
			perror("Desired address is already used");
		}
	}
	else
	{
		perror("No Device with default address present");
	}
}


bool changeFactoryAddress(int &slotPosition)
{
	bool res = false;
	phSensor ph;
	
	if (ph.checkDeviceModell() == true)
	{		
		if (ph.setNewBusAddress(waterSensorAddressSpace[slotPosition] == true))
		{
			res = true;
		}
	}			

	return res;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Determine if a given i2c address is already used by another device.</summary>
///
/// <param name="busAddress">The bus address.</param>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool checkIfAddressIsUsed(int busAddress)
{
	bool res = false;
	int device = initDevice(busAddress);
	std::string request = "";

	if (write(device, request.c_str(), 1) == 1)
	{
		res = true;
	}
	return res;
}

void removePHCircuit(int slotPosition)
{	
	if (checkIfAddressIsUsed(waterSensorAddressSpace[slotPosition]))
	{
		if (!checkIfAddressIsUsed(factoryDefaultPHAddress))
		{
			phSensor ph;
			if (ph.setNewBusAddress(factoryDefaultPHAddress) == true)
			{
				reloadHabitatConfiguration();
			}
		}
		else
		{
			perror("Can't remove device and reset address, because there is a device with the factory default address. Removing would cause address collision");
		}
	}
}

int clearSensorObjects()
{
	int deletedObjects = 0;
	
	clearPHObjects(deletedObjects);
	
	return deletedObjects;
}


void clearPHObjects(int &deleted)
{
	for (int i = 0; i++; i < phSensors.size())
	{
		delete phSensors[i];
		deleted++;
	}
	phSensors.clear();
}

