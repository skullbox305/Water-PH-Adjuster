#include "phControl.h"
#include <iostream>

#include <unistd.h>

using namespace std;

void test()
{	
	int i = 0;
	while (i < 20)
	{
		cout << i++ << endl;
		sleep(1);
	}
}