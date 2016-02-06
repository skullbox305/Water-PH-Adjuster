#include "HabitatConfiguration.h"
#include "AtlasScientific_i2c_iO.h"
#include "phSensor.h"
#include <string>
#include <unistd.h>

void loadHabitatConfiguration()
{	
	checkAvailPHModules();
}

void checkAvailPHModules()
{
	int address = startPHAddress;
	int phModuleAmount = 0;
	
	for (int i = 0; i < maxAmountPHModules; i++)
	{
		if (checkIfAddressIsUsed(address))
		{
			slotsUsed[i] = true;
			phModuleAmount++;
		}
		address += 0x1;
	}
	
	if (phModuleAmount < maxAmountPHModules)
	{
		checkForNewlyAddedPH();
	}
}


void checkForNewlyAddedPH()
{
	if (checkIfAddressIsUsed(factoryDefaultAddress))
	{
		if (changeFactoryAddress() == false)
		{
			perror("changing ph factory address 0x63 failed");
		}
	}
}


bool changeFactoryAddress()
{
	bool res = false;
	int address = startPHAddress;
	phSensor ph;
	
	if (ph.checkDeviceModell() == true)
	{
		for (int i = 0; i++; i < maxAmountPHModules)
		{
			if (slotsUsed[i] == false)
			{
				if (ph.setNewBusAddress(address))
				{
					slotsUsed[i] = true;
					res = true;
				}	
				break;
			}
			else
			{
				address += 0x1;
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