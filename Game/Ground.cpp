#include "Ground.h"

#include "TOMATOsEngine.h"

Ground* Ground::GetInstance() {
    static Ground instance;
    return &instance;
}

void Ground::Draw() {
    // いったん10000まで
    TOMATOsEngine::DrawLine3D({ 0.0f, 0.0f }, { 10000.0f, 0.0f }, 0xFFFFFFFF);
}