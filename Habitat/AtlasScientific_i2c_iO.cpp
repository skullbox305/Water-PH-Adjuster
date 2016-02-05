#include "AtlasScientific_i2c_iO.h"

#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <wiringPiI2C.h>

using namespace std;

bool writeI2C(const string &cmd, int device)
{
	if (write(device, cmd.c_str(), cmd.size()) != (int)cmd.size()) {
		perror("I2C write failed.");
		return false;
	}
	return true;
}

bool readI2C(string &result, int device)
{
	char buffer[20] = { 0 };
	result = "";
	bool res = true;

	if (read(device, buffer, 20) < 1)
	{
		perror("I2C read failed.");
		res = false;
	}
	else
	{
		for (int i = 0; i < 20; i++)//??
			buffer[i] &= 0x7F;

		if (string(buffer)[0] != 1) {
			cout << "Error response: ";
		
			switch (string(buffer)[0]) {
			case 255:
				cout << "No Data" << endl; 
				break;
			case 254: 
				cout << "Pending" << endl; 
				break;
			case 2: cout << "Failed" << endl; 
				break;
			default: 
				cout << "Unknown" << endl;
			}
			res = false;
		}
	}
	result = string(buffer).substr(1);
	return res;
}

int initDevice(int busAddress)
{
	return wiringPiI2CSetup(busAddress);
}


bool takeBusControl(int busAddress)
{
	bool res = false;
	if (wiringPiI2CSetup(busAddress) != -1)
	{
		res = true;
	}
	return res;
}


bool checkIfAddressIsUsed(int busAddress)
{
	bool res = true;
	int device = initDevice(busAddress);
	string request = "";

	if (write(device, request.c_str(), 1) == 1)
	{
		res = false;
	}
	return res;
}
