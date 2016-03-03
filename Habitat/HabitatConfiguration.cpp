#include "HabitatConfiguration.h"
#include "AtlasScientific_i2c_iO.h"
#include "phSensor.h"
#include "phAdjuster.h"

#include <iostream>
#include <fstream>

#include <string>
#include <unistd.h>
#include <stdio.h>
#include <array>
#include <mutex>

using namespace std;

const int maxSlots = 8;
array<int, maxSlots> deviceTypeList;
array<int, maxSlots> deviceObjectIdList;
int currentDeviceAmount;

void loadHabitatConfiguration()
{	
	if (loadDeviceList() == false)
	{
		deviceTypeList.fill(0);
	}
	currentDeviceAmount = 0;
	createDeviceObjects();
}


void reloadHabitatConfiguration()
{

}

void createDeviceObjects()
{
	for (int slotPosition = 1; slotPosition <= deviceTypeList.size(); slotPosition++)
	{
		//später hier multiplexer schalten
		int deviceType = deviceTypeList[slotPosition - 1];
		
		if (deviceType > 0)
		{
			int address = getDeviceDefaultAddress(deviceType);
			if (checkIfAddressIsUsed(address) == true)
			{
				switch (deviceType)
				{
				case PH:
					initPhModule(slotPosition);
					break;	
				case EC:
				
					break;
				case ORP:
				
					break;
				case OXY:
				
					break;
				case PH_ADJUSTER:
					initPHAdjuster(slotPosition);
					break;
				default:
					break;
				}
			}
		}		
	}	
}


void addNewDevice(int slotPosition, int deviceType)
{
	if (checkDeviceType(slotPosition, deviceType) == true)
	{
		deviceTypeList[slotPosition - 1] = deviceType;
	}	
	saveDeviceList();
}


void removeDevice(int slotPosition)
{	
	deviceTypeList[slotPosition - 1] = NO_DEVICE;
	saveDeviceList();
}


bool checkDeviceType(int slotPosition, int deviceType)
{
	//hier später multiplexer auf slotPosition schalten
	int address = getDeviceDefaultAddress(deviceType);
	return checkIfAddressIsUsed(address);	
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


int getDeviceDefaultAddress(int deviceType)
{
	int defaultAddress;
	
	switch (deviceType)
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
	case PH_ADJUSTER:
		defaultAddress = 0x60;
		break;
	default:
		break;
	}
	return defaultAddress;
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



void clearPHAdjObects(int &deleted)
{
	for (int i = 0; i++; i < phAdjusters.size())
	{
		delete phAdjusters[i];
		deleted++;
	}
	phAdjusters.clear();
}


void saveDeviceList()
{
	ofstream outFile("deviceTypeList.bin", ios::out | ios::binary);
	outFile.write((char *) &deviceTypeList, deviceTypeList.size());
	outFile.close();
}


bool loadDeviceList()
{
	bool res = false;
	ifstream inFile("deviceTypeList.bin", ios::in | ios::binary);
	if (inFile.good())
	{
		inFile.read((char *) &deviceTypeList, deviceTypeList.size());
		inFile.close();
		res = true;
	}
	return res;
}
