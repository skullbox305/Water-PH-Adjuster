#include "HabitatConfiguration.h"
#include "i2c_iO.h"
#include "phSensor.h"
#include "phAdjuster.h"
#include "globalMtx.h"

#include <iostream>
#include <fstream>

#include <string>
#include <unistd.h>
#include <stdio.h>

using namespace std;

array<int, maxSlots> deviceTypeList;
array<int, maxSlots> deviceObjectIdList;
int currentDeviceAmount;

void loadHabitatConfiguration()
{	
	if (loadDeviceList() == false)
	{
		deviceTypeList.fill(0);
		system("mkdir -p device_settings"); //testen
	}
	currentDeviceAmount = 0;
	createDeviceObjects();
}


void createDeviceObjects()
{
	for (int slotPosition = 1; slotPosition <= deviceTypeList.size(); slotPosition++)
	{
		switchToSlot(slotPosition, 0);
		int deviceType = deviceTypeList[slotPosition - 1];
		
		if (deviceType > 0)
		{
			int address = getDeviceDefaultAddress(deviceType);
			if (checkIfAddressIsUsed(address) == true)
			{
				deviceObjectIdList[slotPosition - 1] = createDeviceObject(deviceType, slotPosition);
			}
			else
			{
				deviceObjectIdList[slotPosition - 1] = -1;
			}
		}
		else
		{
			deviceObjectIdList[slotPosition - 1] = -1;
		}
	}	
}


int createDeviceObject(int deviceType, int slotPosition)
{
	int objectID;
	
	switch (deviceType)
	{
	case PH:
		objectID = initPhModule(slotPosition);
		break;	
	case EC:
				
		break;
	case ORP:
				
		break;
	case OXY:
				
		break;
	case PH_ADJUSTER:
		objectID = initPHAdjuster(slotPosition);
		break;
	default:
		break;
	}	
	return objectID;
}



void addNewDevice(int slotPosition, int deviceType)
{
	if (checkDeviceType(slotPosition, deviceType) == true)
	{
		deviceTypeList[slotPosition - 1] = deviceType;
		deviceObjectIdList[slotPosition - 1] = createDeviceObject(deviceType, slotPosition);
		saveDeviceList();
	}	
}


void removeDevice(int slotPosition)
{	
	if (deviceObjectIdList[slotPosition - 1] != -1)
	{
		removeDeviceObject(slotPosition);
		deviceObjectIdList[slotPosition - 1] = -1;
	}
	deviceTypeList[slotPosition - 1] = NO_DEVICE;
	
	char path[100];
	snprintf(path, sizeof(path), "device_settings/slot_%d_settings.bin", slotPosition);
	remove(path);
	
	saveDeviceList();
}

void removeDeviceObject(int slotPosition)
{
	int deviceType = deviceTypeList[slotPosition - 1];
	int objectID = deviceObjectIdList[slotPosition - 1];
	
	switch (deviceType)
	{
	case PH:
		delete phSensors[objectID];
		phSensors.erase(phSensors.begin() + objectID);
		break;	
	case EC:
				
		break;
	case ORP:
				
		break;
	case OXY:
				
		break;
	case PH_ADJUSTER:
		delete &phAdjusters[objectID];
		phAdjusters.erase(phAdjusters.begin() + objectID);
		break;
	default:
		break;
	}	
}



bool checkDeviceType(int slotPosition, int deviceType)
{
	busMtx.lock();
	int address = getDeviceDefaultAddress(deviceType);
	switchToSlot(slotPosition, address);
	bool res = checkIfAddressIsUsed(address);
	busMtx.unlock();
	
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
	int device = wiringPiI2CSetup(busAddress);
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


void saveDeviceList()
{
	ofstream outFile("device_settings/deviceTypeList.bin", ios::out | ios::binary);
	outFile.write((char *) &deviceTypeList, sizeof(deviceTypeList));
	outFile.close();
}


bool loadDeviceList()
{
	bool res = false;
	ifstream inFile("device_settings/deviceTypeList.bin", ios::in | ios::binary);
	if (inFile.good())
	{
		inFile.read((char *) &deviceTypeList, sizeof(deviceTypeList));
		inFile.close();
		res = true;
	}
	return res;
}