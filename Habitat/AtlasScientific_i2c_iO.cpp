#include "AtlasScientific_i2c_iO.h"

#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>

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

bool initDevice(int busAddress, int &deviceID)
{
	int device = open("/dev/i2c-1", O_RDWR);
	bool res = true;
	
	if (device < 0) {
		perror("Failed to open the device.");
		res = false;
	}
	else if (ioctl(device, I2C_SLAVE, busAddress) < 0) {
		perror("Unable to set I2C slave address.");
		res = false;
	}
	deviceID = device;
	return res;
}

