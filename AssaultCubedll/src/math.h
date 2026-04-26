#pragma once
extern float vMatrix[4][4];
struct vec2D { float x, y; };
struct pPos { float x, y, z; };

void populateMatrix();
bool worldToScreen(pPos& playerPos, vec2D& vOut, vec2D screenSize);