#include "phSensor.h"
#include "phAdjuster.h"

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
 
int main()
{

	phSensor ph1(0x63);
	cout << ph1.getPHReading() << endl;
	cout << ph1.getCalibrationStatus() << endl;
	return 0;
	
}