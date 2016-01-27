#include "phSensor.h"
#include "phAdjuster.h"

#include <stdio.h>
#include <iostream>
#include <unistd.h>

using namespace std;
 
int main()
{
	phSensor ph1(63);
	cout << ph1.getBusAddress() << endl;
	int test;
	 
	return 0;
	
}