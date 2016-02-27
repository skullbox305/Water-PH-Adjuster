#include "HabitatConfiguration.h"
#include "AtlasScientific_i2c_iO.h"
#include "phSensor.h"
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <array>
#include <mutex>

using namespace std;

const int maxDeviceAmount = 16;
const int startAddress = 3;
array<string, maxDeviceAmount> deviceList;
int currentDeviceAmount;

void loadHabitatConfiguration()
{	
	deviceList.fill("");
	currentDeviceAmount = 0;
	checkAvailModules();
}


void reloadHabitatConfiguration()
{
	if (clearSensorObjects() != currentDeviceAmount)
	{
		perror("Es wurden nicht alle Objekte geloescht!");
	}
	deviceList.fill("");
	currentDeviceAmount = 0;
	checkAvailModules();
}

void checkAvailModules()
{
	int address = startAddress;
	for (int slotPosition = 0; slotPosition < maxDeviceAmount; slotPosition++)
	{
		if (checkIfAddressIsUsed(address))
		{
			if (isPH(address, slotPosition))
			{
				deviceList[slotPosition] = "PH";
				currentDeviceAmount++;
			}
			else
			{
				
			}
		}
		address += 0x1;
	}	
}

bool isPH(int address, int slotPosition)
{
	bool res = false;
	
	phSensor ph(address,0);
	if (ph.checkDeviceModell() == true)
	{
		initPhModule(address, slotPosition + 1); //start with 1, instead of 0
		res = true;
	}
	return res;
}

void addNewCircuit(int slotPosition, int circuitType)
{
	int defaultAddress = getCircuitDefaultAddress(circuitType);
	
	if (checkIfAddressIsUsed(defaultAddress))
	{
		if (deviceList[slotPosition-1].empty())
		{
			if (changeFactoryAddress(slotPosition, circuitType) == true)
			{
				reloadHabitatConfiguration();
			}
			else
			{
				perror("changing factory address failed");
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


bool changeFactoryAddress(int slotPosition, int circuitType)
{
	bool res = false;
	
	if (circuitType == PH)
	{
		phSensor ph;
	
		if (ph.checkDeviceModell() == true)
		{		
			if (ph.setNewBusAddress(startAddress + (slotPosition - 1)) == true)
			{
				res = true;
			}
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

void removeCircuit(int slotPosition, int circuitType)
{	
	int defaultAddress = getCircuitDefaultAddress(circuitType);
	
	if (!deviceList[slotPosition - 1].empty())
	{
		if (!checkIfAddressIsUsed(defaultAddress))
		{
			resetCircuit(circuitType, defaultAddress, slotPosition);
		}
		else
		{
			perror("Can't remove device and reset address, because there is a device with the factory default address present. Removing would cause an address collision");
		}
	}
}

int getCircuitDefaultAddress(int circuitType)
{
	int defaultAddress;
	
	switch (circuitType)
	{
	case PH:
		defaultAddress = 0x63;
		break;	
	case EC:
		defaultAddress = 0x64;
		break;
	case ORP:
		defaultAddress = 0x65;
		break;
	case OXY:
		defaultAddress = 0x66;
		break;
	default:
		break;
	}
	return defaultAddress;
}

void resetCircuit(int circuitType, int defaultAddress, int slotPosition)
{
	if (circuitType == PH)
	{
		phSensor ph((0x3 + (slotPosition - 1)), 0);
		if (ph.setNewBusAddress(defaultAddress) == true)
		{
			reloadHabitatConfiguration();
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
