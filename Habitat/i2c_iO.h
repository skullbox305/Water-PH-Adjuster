#pragma once
#include <iostream>
#include <wiringPiI2C.h>

bool writeI2CAtlasScientific(const std::string &cmd, int device);
bool readI2CAtlasScientific(std::string &result, int device);
int switchToSlot(int slot, int address);
