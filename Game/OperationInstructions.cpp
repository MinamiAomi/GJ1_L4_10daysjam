#include "OperationInstructions.h"

#include "TOMATOSEngine.h"
#include "Easing.h"

// 白線の色
const uint32_t whiteColor = 0x666666FF;

//　---　コントローラー　---
namespace ControllerConstants {
    const Vector3 controllerGraffitiPoints[] = {
        {  0.0f, -1.5f, 0.0f },
        {  1.5f, -1.5f, 0.0f },
        {  3.5f, -3.0f, 0.0f },
        {  5.0f, -3.0f, 0.0f },
        {  3.5f,  1.0f, 0.0f }, // 右上
        {  0.0f,  1.0f, 0.0f }
    };

    const Vector3 stickPosition = { -2.5f, -0.5f, 0.0f };
    const float insideStickSize = 0.4f;
    const float outsideStickSize = 0.7f;
    const float stickSwingValue = 0.2f;

    const Vector3 buttonPosition = { 2.5f, -0.5f, 0.0f };
    const float buttonSize = 0.2f;
    const float buttonDistance = 0.5f;
    const float buttonAnimationSizeFactor = 4.0f;
}

// --- プレイヤー ---
namespace PlayerConstants {
    const float playerOffset = 4.0f;
    const float playerSize = 2.0f;
    const float headSize = playerSize - playerSize / 3.0f;
    const float footSize = playerSize / 3.0f;
    const Vector2 rightFootOffset = { headSize * 0.5f, -headSize * 0.5f };
    const Vector2 leftFootOffset = { -headSize * 0.5f, -headSize * 0.5f };

    const float groundOffset = -playerSize * 0.5f;
    const float groundSize = playerSize * 4.0f;

    const float wallOffset = playerSize * 0.5f;
    const float wallSize = playerSize * 2.0f;

    const float moveAnimeOffset = 1.0f;
    const float jumpAnimeOffset = 2.0f;
    const Vector2 wallKickAnimeOffset = { -2.0f, 2.0f };
    const float hipDropAnimeOffset = 2.0f;

    const Vector2 moveArrowOffset = { 3.0f, playerOffset };
    const float moveArrowRotate = 90.0f * Math::ToRadian;
    const Vector2 jumpArrowOffset = { 3.0f, playerOffset + 1.0f };
    const Vector2 wallKickArrowOffset = { -3.0f, playerOffset };
    const float wallKickArrowRotate = -45.0f * Math::ToRadian;
    const Vector2 hipDropArrowOffset = { 3.0f, playerOffset + 1.0f };
}

namespace BombConstants {
    const uint32_t redColor = 0x550404FF;
    const uint32_t blueColor = 0x004747FF;
    const float bombSize = 2.0f;

    const Vector3 bombOffset = { -5.0f, 1.0f, 0.0f };
    const Vector3 playerOffset = { -5.0f, 4.0f, 0.0f };
    
    const Vector3 upperBombOffset = {-5.0f,  0.5f, 0.0f };
    const Vector3 lowerBombOffset = {-5.0f, -3.0f, 0.0f };

    const Vector2 borderSize = { 2.0f, 8.0f };
    const float arrowOffset = borderSize.y * 0.5f + 2.0f;
    const Vector2 hipDropArrowOffset = { playerOffset.x - 2.0f, playerOffset.y };

    const float borderAnimeOffset = 1.0f;
}

void DrawCircle(const Vector3& translate, float size, const Matrix4x4& matrix, uint32_t color) {
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
        TOMATOsEngine::DrawLine3D(start * matrix, end * matrix, color);
    }
}

void DrawBox(const Vector3& translate, const Vector2& size, const Matrix4x4& matrix, uint32_t color) {
    const Vector2 halfSize = size * 0.5f;
    Vector3 point[] = {
        { -halfSize.x, -halfSize.y, 0.0f },
        { -halfSize.x,  halfSize.y, 0.0f },
        {  halfSize.x,  halfSize.y, 0.0f },
        {  halfSize.x, -halfSize.y, 0.0f },
    };

    for (int i = 0; i < _countof(point); i++) {
        point[i] = (point[i] + translate) * matrix;
    }

    TOMATOsEngine::DrawLine3D(point[0], point[1], color);
    TOMATOsEngine::DrawLine3D(point[1], point[2], color);
    TOMATOsEngine::DrawLine3D(point[2], point[3], color);
    TOMATOsEngine::DrawLine3D(point[3], point[0], color);
}

void DrawArrow(const Vector3& translate, const Vector2& size, float radian, const Matrix4x4& matrix, uint32_t color) {
    Vector3 point[] = {
        {  0.0f,  1.0f, 0.0f },
        {  0.0f, -1.0f, 0.0f },
        {  0.5f,  0.0f, 0.0f },
        { -0.5f,  0.0f, 0.0f },
    };


    float s = std::sin(radian);
    float c = std::cos(radian);

    for (int i = 0; i < _countof(point); i++) {
        point[i].x = point[i].x * size.x;
        point[i].y = point[i].y * size.y;

        float x = point[i].x;
        float y = point[i].y;

        point[i].x = x * c + y * s + translate.x;
        point[i].y = x * -s + y * c + translate.y;
        point[i].z = translate.z;

        point[i] = point[i] * matrix;
    }

    TOMATOsEngine::DrawLine3D(point[0], point[1], color);
    TOMATOsEngine::DrawLine3D(point[0], point[2], color);
    TOMATOsEngine::DrawLine3D(point[0], point[3], color);
}

void OperationInstructions::Draw(const Vector3& translate, const Vector3& scale, Type type) {

    t += 1.0f / 1000.0f;
    if (t >= 1.0f) {
        t -= 1.0f;
    }

    Matrix4x4 matrix = Matrix4x4::MakeAffineTransform(scale, Quaternion::identity, translate);

    switch (type) {
    case OperationInstructions::Type::Move:
        DrawController({}, Vector3::one, matrix, ControllerAnimation::Stick);
        DrawPlayer({}, Vector3::one, matrix, PlayerAnimation::Move);
        break;
    case OperationInstructions::Type::Jump:
        DrawController({}, Vector3::one, matrix, ControllerAnimation::AButton);
        DrawPlayer({}, Vector3::one, matrix, PlayerAnimation::Jump);
        break;
    case OperationInstructions::Type::WallKick:
        DrawController({}, Vector3::one, matrix, ControllerAnimation::AButton);
        DrawPlayer({}, Vector3::one, matrix, PlayerAnimation::WallKick);
        break;
    case OperationInstructions::Type::HipDrop:
        DrawController({}, Vector3::one, matrix, ControllerAnimation::BButton);
        DrawPlayer({}, Vector3::one, matrix, PlayerAnimation::HipDrop);
        break;
    case OperationInstructions::Type::RedBomb:
        // ボーダー
    {
        float borderAnimeT = t;
        float borderAnimeOffset = Math::Lerp(borderAnimeT, -BombConstants::borderAnimeOffset, BombConstants::borderAnimeOffset);

        Vector2 halfSize = { BombConstants::borderSize.x * 0.5f, BombConstants::borderSize.y * 0.5f };
        Vector3 point[] = {
                { -halfSize.x, -halfSize.y, 0.0f },
                { -halfSize.x,  halfSize.y, 0.0f },
                {  halfSize.x,  halfSize.y, 0.0f },
                {  halfSize.x, -halfSize.y, 0.0f },
        };
        for (int i = 0; i < _countof(point); i++) {
            point[i].x = point[i].x + borderAnimeOffset;
            point[i] = point[i] * matrix;
        }
        TOMATOsEngine::DrawLine3D(point[0], point[1], whiteColor);
        TOMATOsEngine::DrawLine3D(point[2], point[3], whiteColor);
    }
    DrawBox(BombConstants::bombOffset, { BombConstants::bombSize, BombConstants::bombSize }, matrix, BombConstants::redColor);
    DrawArrow({ 0.0f, BombConstants::arrowOffset, 0.0f }, { 1.0f, 1.0f }, 90.0f * Math::ToRadian, matrix, BombConstants::redColor);
    break;
    case OperationInstructions::Type::BlueBomb:
        // ボーダー
        // ボーダー
    {
        float borderAnimeT = t;
        float borderAnimeOffset = Math::Lerp(borderAnimeT, BombConstants::borderAnimeOffset, -BombConstants::borderAnimeOffset);

        Vector2 halfSize = { BombConstants::borderSize.x * 0.5f, BombConstants::borderSize.y * 0.5f };
        Vector3 point[] = {
                { -halfSize.x, -halfSize.y, 0.0f },
                { -halfSize.x,  halfSize.y, 0.0f },
                {  halfSize.x,  halfSize.y, 0.0f },
                {  halfSize.x, -halfSize.y, 0.0f },
        };
        for (int i = 0; i < _countof(point); i++) {
            point[i].x = point[i].x + borderAnimeOffset;
            point[i] = point[i] * matrix;
        }
        TOMATOsEngine::DrawLine3D(point[0], point[1], whiteColor);
        TOMATOsEngine::DrawLine3D(point[2], point[3], whiteColor);
    }
    DrawBox(BombConstants::bombOffset, { BombConstants::bombSize, BombConstants::bombSize }, matrix, BombConstants::blueColor);
    DrawArrow({ 0.0f, BombConstants::arrowOffset, 0.0f }, { 1.0f, 1.0f }, -90.0f * Math::ToRadian, matrix, BombConstants::blueColor);

    break;
    case OperationInstructions::Type::Burst:
    {
        float borderAnimeT = t;
        float borderAnimeOffset = Math::Lerp(borderAnimeT, -BombConstants::borderAnimeOffset * 2.0f, BombConstants::borderAnimeOffset * 2.0f);

        Vector2 halfSize = { BombConstants::borderSize.x * 0.5f, BombConstants::borderSize.y * 0.5f };
        Vector3 point[] = {
                { -halfSize.x, -halfSize.y, 0.0f },
                { -halfSize.x,  halfSize.y, 0.0f },
                {  halfSize.x,  halfSize.y, 0.0f },
                {  halfSize.x, -halfSize.y, 0.0f },
        };
        for (int i = 0; i < _countof(point); i++) {
            point[i].x = point[i].x + borderAnimeOffset;
            point[i] = point[i] * matrix;
        }
        TOMATOsEngine::DrawLine3D(point[0], point[1], whiteColor);
        TOMATOsEngine::DrawLine3D(point[2], point[3], whiteColor);
    }
    DrawBox(BombConstants::upperBombOffset, { BombConstants::bombSize, BombConstants::bombSize }, matrix, BombConstants::redColor);
    DrawBox(BombConstants::lowerBombOffset, { BombConstants::bombSize, BombConstants::bombSize }, matrix, BombConstants::blueColor);

    DrawBox({ BombConstants::playerOffset.x, BombConstants::playerOffset.y, 0.0f }, { PlayerConstants::headSize, PlayerConstants::headSize }, matrix, whiteColor);
    DrawBox({ BombConstants::playerOffset.x + PlayerConstants::rightFootOffset.x, PlayerConstants::rightFootOffset.y + BombConstants::playerOffset.y, 0.0f }, { PlayerConstants::footSize, PlayerConstants::footSize }, matrix, whiteColor);
    DrawBox({ BombConstants::playerOffset.x + PlayerConstants::leftFootOffset.x, PlayerConstants::leftFootOffset.y + BombConstants::playerOffset.y, 0.0f }, { PlayerConstants::footSize, PlayerConstants::footSize }, matrix, whiteColor);

    DrawArrow({ 0.0f, BombConstants::arrowOffset, 0.0f }, { 1.0f, 2.0f }, 90.0f * Math::ToRadian, matrix, whiteColor);
    DrawArrow({ BombConstants::hipDropArrowOffset.x, BombConstants::hipDropArrowOffset.y, 0.0f }, { 1.0f, 1.0f }, 180.0f * Math::ToRadian, matrix, whiteColor);
    // ボーダー
    break;
    default:
        break;
    }
}

void OperationInstructions::DrawController(const Vector3& position, const Vector3& scale, const Matrix4x4& parentMatrix, ControllerAnimation controllerAnimation) {

    Matrix4x4 matrix = Matrix4x4::MakeAffineTransform(scale, Quaternion::identity, position) * parentMatrix;

    // Controller
    for (int i = 0; i < _countof(ControllerConstants::controllerGraffitiPoints) - 1; i++) {
        int j = i + 1;
        Vector3 start = ControllerConstants::controllerGraffitiPoints[i];
        Vector3 end = ControllerConstants::controllerGraffitiPoints[j];
        TOMATOsEngine::DrawLine3D(start * matrix, end * matrix, whiteColor);
        start.x = -start.x;
        end.x = -end.x;
        TOMATOsEngine::DrawLine3D(start * matrix, end * matrix, whiteColor);
    }

    // stick
    DrawCircle(ControllerConstants::stickPosition, ControllerConstants::outsideStickSize, matrix, whiteColor);

    DrawCircle(ControllerConstants::buttonPosition + Vector3{ -ControllerConstants::buttonDistance, 0.0f, 0.0f }, ControllerConstants::buttonSize, matrix, whiteColor);
    DrawCircle(ControllerConstants::buttonPosition + Vector3{ 0.0f, ControllerConstants::buttonDistance, 0.0f }, ControllerConstants::buttonSize, matrix, whiteColor);

    float controllerAnimeT = 1.0f - std::abs(t * 2.0f - 1.0f);
    switch (controllerAnimation) {
    case OperationInstructions::ControllerAnimation::Stick: {
        Vector3 stickAnimeOffset = { Math::Lerp(controllerAnimeT, -ControllerConstants::stickSwingValue, ControllerConstants::stickSwingValue), 0.0f, 0.0f };
        DrawCircle(ControllerConstants::stickPosition + stickAnimeOffset, ControllerConstants::insideStickSize, matrix, whiteColor);
        DrawCircle(ControllerConstants::buttonPosition + Vector3{ ControllerConstants::buttonDistance, 0.0f, 0.0f }, ControllerConstants::buttonSize, matrix, whiteColor);
        DrawCircle(ControllerConstants::buttonPosition + Vector3{ 0.0f, -ControllerConstants::buttonDistance, 0.0f }, ControllerConstants::buttonSize, matrix, whiteColor);

        break;
    }
    case OperationInstructions::ControllerAnimation::AButton: {
        // button
        float buttonAnimeSize = Math::Lerp(controllerAnimeT, ControllerConstants::buttonSize, ControllerConstants::buttonSize * ControllerConstants::buttonAnimationSizeFactor);
        float buttonAnimeOffset = Math::Lerp(controllerAnimeT, 0.0f, ControllerConstants::buttonSize * ControllerConstants::buttonAnimationSizeFactor * 0.5f);
        DrawCircle(ControllerConstants::stickPosition, ControllerConstants::insideStickSize, matrix, whiteColor);
        DrawCircle(ControllerConstants::buttonPosition + Vector3{ ControllerConstants::buttonDistance, 0.0f, 0.0f }, ControllerConstants::buttonSize, matrix, whiteColor);
        DrawCircle(ControllerConstants::buttonPosition + Vector3{ 0.0f, -ControllerConstants::buttonDistance - buttonAnimeOffset, 0.0f }, buttonAnimeSize, matrix, whiteColor);
        break;
    }
    case OperationInstructions::ControllerAnimation::BButton: {
        // button
        float buttonAnimeSize = Math::Lerp(controllerAnimeT, ControllerConstants::buttonSize, ControllerConstants::buttonSize * ControllerConstants::buttonAnimationSizeFactor);
        float buttonAnimeOffset = Math::Lerp(controllerAnimeT, 0.0f, ControllerConstants::buttonSize * ControllerConstants::buttonAnimationSizeFactor * 0.5f);
        DrawCircle(ControllerConstants::stickPosition, ControllerConstants::insideStickSize, matrix, whiteColor);
        DrawCircle(ControllerConstants::buttonPosition + Vector3{ ControllerConstants::buttonDistance + buttonAnimeOffset, 0.0f, 0.0f }, buttonAnimeSize, matrix, whiteColor);
        DrawCircle(ControllerConstants::buttonPosition + Vector3{ 0.0f, -ControllerConstants::buttonDistance, 0.0f }, ControllerConstants::buttonSize, matrix, whiteColor);
        break;
    }
    default:
        assert(true);
        break;
    }

}

void OperationInstructions::DrawPlayer(const Vector3& position, const Vector3& scale, const Matrix4x4& parentMatrix, PlayerAnimation playerAnimation) {

    Matrix4x4 matrix = Matrix4x4::MakeAffineTransform(scale, Quaternion::identity, position) * parentMatrix;

    switch (playerAnimation) {
    case OperationInstructions::PlayerAnimation::Move: {
        float playerAnimeT = 1.0f - std::abs(t * 2.0f - 1.0f);
        float playerAnimeOffset = Math::Lerp(playerAnimeT, -PlayerConstants::moveAnimeOffset, PlayerConstants::moveAnimeOffset);

        // プレイヤー
        DrawBox({ playerAnimeOffset, PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::headSize, PlayerConstants::headSize }, matrix, whiteColor);
        DrawBox({ playerAnimeOffset + PlayerConstants::rightFootOffset.x, PlayerConstants::rightFootOffset.y + PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::footSize, PlayerConstants::footSize }, matrix, whiteColor);
        DrawBox({ playerAnimeOffset + PlayerConstants::leftFootOffset.x, PlayerConstants::leftFootOffset.y + PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::footSize, PlayerConstants::footSize }, matrix, whiteColor);
        // 床
        Vector3 groundStart = { -PlayerConstants::groundSize * 0.5f, PlayerConstants::groundOffset + PlayerConstants::playerOffset, 0.0f };
        Vector3 groundEnd = { PlayerConstants::groundSize * 0.5f, PlayerConstants::groundOffset + PlayerConstants::playerOffset, 0.0f };
        TOMATOsEngine::DrawLine3D(groundStart * matrix, groundEnd * matrix, whiteColor);
        // 矢印
        DrawArrow({ PlayerConstants::moveArrowOffset.x, PlayerConstants::moveArrowOffset.y, 0.0f }, Vector2::one, PlayerConstants::moveArrowRotate, matrix, whiteColor);
        DrawArrow({ -PlayerConstants::moveArrowOffset.x, PlayerConstants::moveArrowOffset.y, 0.0f }, Vector2::one, -PlayerConstants::moveArrowRotate, matrix, whiteColor);
        break;
    }
    case OperationInstructions::PlayerAnimation::Jump: {
        float playerAnimeT = Easing::OutQuad(1.0f - std::abs(t * 2.0f - 1.0f));
        float playerAnimeOffset = Math::Lerp(playerAnimeT, 0.0f, PlayerConstants::jumpAnimeOffset);

        // プレイヤー
        DrawBox({ 0.0f, playerAnimeOffset + PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::headSize, PlayerConstants::headSize }, matrix, whiteColor);
        DrawBox({ PlayerConstants::rightFootOffset.x, playerAnimeOffset + PlayerConstants::rightFootOffset.y + PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::footSize, PlayerConstants::footSize }, matrix, whiteColor);
        DrawBox({ PlayerConstants::leftFootOffset.x, playerAnimeOffset + PlayerConstants::leftFootOffset.y + PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::footSize, PlayerConstants::footSize }, matrix, whiteColor);
        // 床
        Vector3 groundStart = { -PlayerConstants::groundSize * 0.5f, PlayerConstants::groundOffset + PlayerConstants::playerOffset, 0.0f };
        Vector3 groundEnd = { PlayerConstants::groundSize * 0.5f, PlayerConstants::groundOffset + PlayerConstants::playerOffset, 0.0f };
        TOMATOsEngine::DrawLine3D(groundStart * matrix, groundEnd * matrix, whiteColor);
        //矢印
        DrawArrow({ PlayerConstants::jumpArrowOffset.x, PlayerConstants::jumpArrowOffset.y, 0.0f }, Vector2::one, 0.0f, matrix, whiteColor);

        break;
    }
    case OperationInstructions::PlayerAnimation::WallKick: {
        float playerAnimeT = Easing::OutQuad(1.0f - std::abs(t * 2.0f - 1.0f));
        Vector2 playerAnimeOffset = Vector2::Lerp(playerAnimeT, Vector2::zero, PlayerConstants::wallKickAnimeOffset);


        DrawBox({ playerAnimeOffset.x, playerAnimeOffset.y + PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::headSize, PlayerConstants::headSize }, matrix, whiteColor);
        DrawBox({ playerAnimeOffset.x + PlayerConstants::rightFootOffset.x, playerAnimeOffset.y + PlayerConstants::rightFootOffset.y + PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::footSize, PlayerConstants::footSize }, matrix, whiteColor);
        DrawBox({ playerAnimeOffset.x + PlayerConstants::leftFootOffset.x, playerAnimeOffset.y + PlayerConstants::leftFootOffset.y + PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::footSize, PlayerConstants::footSize }, matrix, whiteColor);

        Vector3 wallStart = { PlayerConstants::wallOffset, -PlayerConstants::wallSize * 0.5f + PlayerConstants::playerOffset, 0.0f };
        Vector3 wallEnd = { PlayerConstants::wallOffset, PlayerConstants::wallSize * 0.5f + PlayerConstants::playerOffset, 0.0f };
        TOMATOsEngine::DrawLine3D(wallStart * matrix, wallEnd * matrix, whiteColor);

        //矢印
        DrawArrow({ PlayerConstants::wallKickArrowOffset.x, PlayerConstants::wallKickArrowOffset.y, 0.0f }, Vector2::one, PlayerConstants::wallKickArrowRotate, matrix, whiteColor);

        break;
    }
    case OperationInstructions::PlayerAnimation::HipDrop: {
        float playerAnimeT = Easing::InBack(1.0f - std::abs(t * 2.0f - 1.0f));
        float playerAnimeOffset = Math::Lerp(playerAnimeT, PlayerConstants::hipDropAnimeOffset, 0.0f);

        DrawBox({ 0.0f, playerAnimeOffset + PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::headSize, PlayerConstants::headSize }, matrix, whiteColor);
        DrawBox({ PlayerConstants::rightFootOffset.x, playerAnimeOffset + PlayerConstants::rightFootOffset.y + PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::footSize, PlayerConstants::footSize }, matrix, whiteColor);
        DrawBox({ PlayerConstants::leftFootOffset.x, playerAnimeOffset + PlayerConstants::leftFootOffset.y + PlayerConstants::playerOffset, 0.0f }, { PlayerConstants::footSize, PlayerConstants::footSize }, matrix, whiteColor);

        Vector3 groundStart = { -PlayerConstants::groundSize * 0.5f, PlayerConstants::groundOffset + PlayerConstants::playerOffset, 0.0f };
        Vector3 groundEnd = { PlayerConstants::groundSize * 0.5f, PlayerConstants::groundOffset + PlayerConstants::playerOffset, 0.0f };
        TOMATOsEngine::DrawLine3D(groundStart * matrix, groundEnd * matrix, whiteColor);

        //矢印
        DrawArrow({ PlayerConstants::hipDropArrowOffset.x, PlayerConstants::hipDropArrowOffset.y, 0.0f }, Vector2::one, Math::Pi, matrix, whiteColor);

        break;
    }
    default:
        break;
    }

}
