#pragma once
#include <iostream>

bool writeI2C(const std::string &cmd, int device);
bool readI2C(std::string &result, int device);
int initDevice(int busAddress);
bool takeBusControl(int busAddress); //glaube überflüssig, da device eigentlich eindeutig
