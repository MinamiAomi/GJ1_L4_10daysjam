#pragma once

#include <array>
#include <vector>

#include "Math/MathUtils.h"
#include "TextureHandle.h"
#include "PlayerModel.h"

class Player {
public:

	void Initialize();
	void Update();
	void Draw();

	void SetPosition(const Vector2& position) { position_ = position; }
	bool GetFacing() const { return isFacing; }
	Vector2 GetSize()const { return size_; }
	Vector2 GetPosition()const { return position_; }
	Vector2 GetVelocity() const { return velocity_; }
	float GetRotate()const { return rotate_; }
	bool GetIsHipDrop() const { return isHipDrop_; }
private:

	void Move();
	void HipDrop();
	void CheckCollisions();

	PlayerModel playerModel_;

	Vector2 position_;
	Vector2 wallToPosition_;
	float rotate_;
	Vector2 velocity_;
	Vector2 size_;

	bool isOnGround_ = false;
	bool isWallSliding_ = false;
	bool isFacing = true; // 向いている方向(false 左,true 右 )
	int wallDirection_ = 0; // 壁の方向 (-1 左, 1 右, 0 なし)
	bool isJumping_ = false;
	bool isHipDrop_ = false;

	Vector4 playerParticleColor_;

	const float gravity_ = -0.015f;
	const float moveAcceleration_ = 0.06f;
	const float airAcceleration_ = 0.03f;
	const float maxMoveSpeed_ = 0.45f;
	const float friction_ = 0.9f;
	const float jumpPower_ = 0.8f;
	const float jumpStopForceMultiplier_ = 0.5f;
	const float wallSlideSpeed_ = -0.15f;
	const Vector2 wallJumpPower_ = { 0.75f,0.65f };
	const float hipDropRotateSpeed_ = 36.0f * Math::ToRadian;
	const float hipDropUpSpeed_ = 0.1f;
	const float hipDropSpeed_ = 1.8f;
};