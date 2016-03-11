#pragma once
#include <array>

// Device type IDs
#define NO_DEVICE 0
#define PH 1
#define EC 2
#define ORP 3
#define OXY 4
#define PH_ADJUSTER 5

void loadHabitatConfiguration();
void createDeviceObjects();
int createDeviceObject(int deviceType, int slotPosition);
void removeDeviceObject(int slotPosition);
void addNewDevice(int slotPosition, int deviceType);
bool checkDeviceType(int slotPosition, int deviceType);
void removeDevice(int slotPosition);
void saveDeviceList();
bool loadDeviceList();
int getDeviceDefaultAddress(int deviceType);
bool checkIfAddressIsUsed(int busAddress);

const int maxSlots = 8;
extern std::array<int, maxSlots> deviceObjectIdList;


