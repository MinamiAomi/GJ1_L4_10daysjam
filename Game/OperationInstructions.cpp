#include "OperationInstructions.h"

#include "TOMATOSEngine.h"

const Vector3 controllerGraffitiPoints[] = {
    {  0.0f, -0.5f, 0.0f },
    {  1.5f, -0.5f, 0.0f },
    {  3.5f, -2.0f, 0.0f }, 
    {  5.0f, -2.0f, 0.0f },
    {  3.5f,  2.0f, 0.0f }, // 右上
    {  0.0f,  2.0f, 0.0f }    
};


const Vector3 stickPosition = { -2.5f, 0.5f, 0.0f};
const float insideStickSize = 0.4f;
const float outsideStickSize = 0.7f;

const Vector3 buttonPosition = { 2.5f, 0.5f, 0.0f};
const float buttonSize = 0.2f;
const float buttonDistance = 0.5f;


void DrawCircle(const Vector3& translate, float size, const Matrix4x4& matrix) {
    const int kDivisionCount = 16;
    const float angleDifference = Math::TwoPi / kDivisionCount;
    for (int i = 0; i < kDivisionCount; ++i) {
        int j = (i + 1) % kDivisionCount;
        float angleStart = angleDifference * i;
        float angleEnd = angleDifference * j;

        Vector3 startOriginal = { std::cos(angleStart) * size, std::sin(angleStart) * size, 0.0f };
        Vector3 endOriginal = { std::cos(angleEnd) * size, std::sin(angleEnd) * size, 0.0f };
        Vector3 start = startOriginal + translate;
        Vector3 end = endOriginal + translate;
        TOMATOsEngine::DrawLine3D(start * matrix, end * matrix, 0xFFFFFFFF);
    }
}

void OperationInstructions::Draw(const Vector3& translate, const Vector3& scale) {
    
    static float t = 0.0f;
    t += 1.0f / 60.0f;
    if (t >= 1.0f) {
        t -= 1.0f;
    }

    Matrix4x4 matrix = Matrix4x4::MakeAffineTransform(scale, Quaternion::identity, translate);

    // Controller
    for (int i = 0; i < _countof(controllerGraffitiPoints) - 1; i++) {
        int j = i + 1;
        Vector3 start = controllerGraffitiPoints[i];
        Vector3 end = controllerGraffitiPoints[j];
        TOMATOsEngine::DrawLine3D(start * matrix, end * matrix, 0xFFFFFFFF);
        start.x = -start.x;
        end.x = -end.x;
        TOMATOsEngine::DrawLine3D(start * matrix, end * matrix, 0xFFFFFFFF);
    }

    // stick
    Vector3 stickAnimeOffset = { Math::Lerp(1.0f - std::abs(t * 2.0f - 1.0f), -0.2f, 0.2f), 0.0f, 0.0f};
    DrawCircle(stickPosition + stickAnimeOffset, insideStickSize, matrix);
    DrawCircle(stickPosition, outsideStickSize, matrix);

    // button

    DrawCircle(buttonPosition + Vector3{buttonDistance, 0.0f, 0.0f}, buttonSize, matrix);
    DrawCircle(buttonPosition + Vector3{0.0f, -buttonDistance, 0.0f}, buttonSize, matrix);
    DrawCircle(buttonPosition + Vector3{-buttonDistance, 0.0f, 0.0f}, buttonSize, matrix);
    DrawCircle(buttonPosition + Vector3{0.0f, buttonDistance, 0.0f}, buttonSize, matrix);

}