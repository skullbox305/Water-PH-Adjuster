#pragma once

void loadHabitatConfiguration();
void reloadHabitatConfiguration();
void checkAvailModules();
void checkAvailPHModules();
void addNewPHCircuit(int newAddr);
void removePHCircuit(int slotPosition);
bool changeFactoryAddress(int &slotPosition);
bool checkIfAddressIsUsed(int busAddress);

int clearSensorObjects();
void clearPHObjects(int &deleted);


