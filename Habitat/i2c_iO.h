#pragma once
#include <iostream>

int writeI2C(const std::string &cmd, int device);
int readI2C(std::string &result, int device);
int initDevice(int busAddress);
