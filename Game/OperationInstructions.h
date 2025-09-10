#pragma once

#include "Math/Mathutils.h"

class OperationInstructions {
public:
    enum class Type {
        Move,  // 移動
        Jump,  // ジャンプ
        WallKick,  // 壁キック
        HipDrop,   // ヒップドロップ
        RedBomb,   // 赤爆弾
        BlueBomb,  // 青爆弾
        Burst, // バースト(ヒップドロップ破壊)
    };

    void Draw(const Vector3& translate, const Vector3& scale, Type type);

private:
    float t = 0.0f;

    enum class ControllerAnimation {
        Stick,
        AButton,
        BButton
    };

    enum class PlayerAnimation {
        Move,
        Jump,
        WallKick,
        HipDrop
    };

    void DrawController(const Vector3& position, const Vector3& scale, const Matrix4x4& matrix, ControllerAnimation controllerAnimation);
    void DrawPlayer(const Vector3& position, const Vector3& scale, const Matrix4x4& matrix, PlayerAnimation playerAnimation);
    void DrawBomb(const Vector3& position, const Vector3& scale, const Matrix4x4& matrix);
};