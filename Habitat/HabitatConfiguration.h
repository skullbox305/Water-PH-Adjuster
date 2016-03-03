#pragma once

// Device type IDs
#define NO_DEVICE 0
#define PH 1
#define EC 2
#define ORP 3
#define OXY 4
#define PH_ADJUSTER 5

void loadHabitatConfiguration();
void reloadHabitatConfiguration();
void createDeviceObjects();
void addNewDevice(int slotPosition, int deviceType);
bool checkDeviceType(int slotPosition, int deviceType);
void removeDevice(int slotPosition);
void saveDeviceList();
bool loadDeviceList();
int getDeviceDefaultAddress(int deviceType);
bool checkIfAddressIsUsed(int busAddress);

int clearSensorObjects();
void clearPHObjects(int &deleted);
void clearPHAdjObects(int &deleted);


