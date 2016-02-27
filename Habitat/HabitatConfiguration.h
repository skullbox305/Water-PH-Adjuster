#pragma once

// Circuit type IDs
#define PH 1
#define EC 2
#define ORP 3
#define OXY 4

void loadHabitatConfiguration();
void reloadHabitatConfiguration();
void checkAvailModules();
bool isPH(int address, int slotPosition);
void addNewCircuit(int slotPosition, int circuitType);
void removeCircuit(int slotPosition, int circuitType);
void resetCircuit(int circuitType, int defaultAddress, int slotPosition);
int getCircuitDefaultAddress(int circuitType);
bool changeFactoryAddress(int slotPosition, int circuitType);
bool checkIfAddressIsUsed(int busAddress);

int clearSensorObjects();
void clearPHObjects(int &deleted);


