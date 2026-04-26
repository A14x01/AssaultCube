#include "math.h"
#include "constant.h"

float vMatrix[4][4];

void populateMatrix() {
    float* matrixAddr = (float*)Offsets::ViewMatrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            vMatrix[i][j] = matrixAddr[j * 4 + i];
        }
    }
}

bool worldToScreen(pPos& playerPos, vec2D& vOut, vec2D screenSize) {

    float View = 0.f;
    float SightX = screenSize.x / 2, SightY = screenSize.y / 2;

    View = vMatrix[3][0] * playerPos.x + vMatrix[3][1] * playerPos.y + vMatrix[3][2] * playerPos.z + vMatrix[3][3];

    if (View <= 0.01)
        return false;

    vOut.x = SightX + (vMatrix[0][0] * playerPos.x + vMatrix[0][1] * playerPos.y + vMatrix[0][2] * playerPos.z + vMatrix[0][3]) / View * SightX;
    vOut.y = SightY - (vMatrix[1][0] * playerPos.x + vMatrix[1][1] * playerPos.y + vMatrix[1][2] * playerPos.z + vMatrix[1][3]) / View * SightY;

    return true;
}