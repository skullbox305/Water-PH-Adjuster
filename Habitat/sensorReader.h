#pragma once
#include "phSensor.h"
#include <mutex>

void readPH();
void readSensors();
void startSensorReader(std::mutex &mtx);
