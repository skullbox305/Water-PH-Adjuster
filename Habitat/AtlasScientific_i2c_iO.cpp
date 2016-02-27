////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	AtlasScientific_i2c_iO.cpp
//
// summary:	Implements the atlas scientific i 2c i o class
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "AtlasScientific_i2c_iO.h"

#include <string.h>
#include <unistd.h>
#include <wiringPiI2C.h>

using namespace std;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Writes a command string to a given i2c device.</summary>
///
/// <param name="cmd">   The command string i.e "Cal,mid,7.0".</param>
/// <param name="device">The device.</param>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool writeI2C(const string &cmd, int device)
{
	bool res = false;
	if (write(device, cmd.c_str(), cmd.size()) == (int)cmd.size()) {
		res = true;
	}
	return res;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Reads the response of a given i2c device.</summary>
///
/// <param name="result">[in,out] The result of the request.</param>
/// <param name="device">The device.</param>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Initialises the i2c device.</summary>
///
/// <param name="busAddress">The i2c bus address of the device.</param>
///
/// <returns>The device.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
int initDevice(int busAddress)
{
	return wiringPiI2CSetup(busAddress);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>Take bus control. Not sure yet, how it will work under real condition, because I
/// 	currently just have one module at hand.</summary>
///
/// <param name="busAddress">The i2c bus address of the device.</param>
///
/// <returns>true if it succeeds, false if it fails.</returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool takeBusControl(int busAddress)
{
	bool res = false;
	if (wiringPiI2CSetup(busAddress) != -1)
	{
		res = true;
	}
	return res;
}
