#pragma once
#include "phSensor.h"
#include <mutex>

void initSensorReader();
void readPH();
void readSensors();
void startSensorReader(std::mutex &mtx);
