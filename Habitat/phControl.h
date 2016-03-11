#pragma once

void startPHControl();
void adjustPH();
void adjustPHOne(int phAdjObjectID);
void adjustPHTwo(int phAdjObjectID);

void adjustPHDown(int phAdjObjectID, int adjusterID);
void adjustPHUp(int phAdjObjectID, int adjusterID);

void getCurrentTargetPH(int phAdjObjectID, int adjusterID);