#include "Ground.h"

#include "TOMATOsEngine.h"
#include "Wall.h"

Ground* Ground::GetInstance() {
    static Ground instance;
    return &instance;
}

void Ground::Draw() {

    auto wall = Wall::GetInstance();
    auto wallPosition = wall->GetPosition();

    // いったん10000まで
    TOMATOsEngine::DrawLine3D({ wallPosition - 100.0f, 0.0f }, { wallPosition + 1000.0f, 0.0f }, 0xFFFFFFFF);
}