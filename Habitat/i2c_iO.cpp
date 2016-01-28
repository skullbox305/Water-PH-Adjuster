#include "i2c_iO.h"

#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>

using namespace std;

int writeI2C(const string &cmd, int device)
{
	if (write(device, cmd.c_str(), cmd.size()) != (int)cmd.size()) {
		perror("I2C write failed.");
		return -1;
	}
	return 0;
}

int readI2C(string &result, int device)
{
	char buf[20] = { 0 };
	result = "";

	if (read(device, buf, 20) < 1) {
		perror("I2C read failed.");
		return -1;
	}
	
	for (int byte = 0; byte < 20; byte++)
		buf[byte] &= 0x7F;

	if (string(buf)[0] != 1) {
		cout << "Error response: ";
		
		switch ((unsigned char)result[0]) {
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
		return -1;
	}

	result = string(buf).substr(1);
	return 0;
}

int initDevice(int busAddress)
{
	int device = open("/dev/i2c-1", O_RDWR);
	
	if (device < 0) {
		perror("Failed to open the device.");
		return -1;
	}
	
	if (ioctl(device, I2C_SLAVE, busAddress) < 0) {
		perror("Unable to set I2C slave address.");
		return -1;
	}
	return device;
}

