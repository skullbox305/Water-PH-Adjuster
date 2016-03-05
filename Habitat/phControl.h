#pragma once

void startPHControl();
void adjustPH();
void fillTubesIfNot(int phAdjObjectID);
void fillTubes(int phAdjObjectID, int adjusterID);
void initPHDown(int phAdjObjectID, int adjusterID);
void initPHUp(int phAdjObjectID, int adjusterID);