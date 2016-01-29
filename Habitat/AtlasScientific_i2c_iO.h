#pragma once
#include <iostream>

bool writeI2C(const std::string &cmd, int device);
bool readI2C(std::string &result, int device);
bool initDevice(int busAddress, int &deviceID);
