#pragma once

#include <array>
#include <vector>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class Field;
class ParticleManager;

class Player {
public:

	void Initialize();
	void Update();
	void Draw();

	void SetPosition(const Vector2& position) { position_ = position; }
	
private:

	void CheckCollisions();
	
	Vector2 position_;
	Vector2 velocity_;
	Vector2 size_;

	bool isOnGround_ = false;
	bool isWallSliding_ = false;
	int wallDirection_ = 0; // 壁の方向 (-1 左, 1 右, 0 なし)

	const float gravity_ = -0.25f;
	const float moveAcceleration_ = 0.4f;
	const float airAcceleration_ = 0.2f;
	const float maxMoveSpeed_ = 4.5f;
	const float friction_ = 0.9f;
	const float jumpPower_ = 8.5f;
	const float wallSlideSpeed_ = -1.5f;
	const Vector2 wallJumpPower_ = { 6.0f,6.5f };
};