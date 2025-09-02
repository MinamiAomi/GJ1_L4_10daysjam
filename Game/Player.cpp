#include "Player.h"

#include "TOMATOsEngine.h"
#include "Math/Color.h"
#include "Particle/ParticleManager.h"
#include "Math/Random.h"

#include "Easing.h"

void Player::Initialize() {
	position_ = Vector2::zero;
	velocity_ = Vector2::zero;
	size_ = { 30.0f, 30.0f };
}

void Player::Update() {
	const auto& pad = TOMATOsEngine::GetGamePadState();
	const auto prepad = TOMATOsEngine::GetGamePadPreState();

	Vector2 move = Vector2::zero;

	if (TOMATOsEngine::IsKeyPressed(DIK_D) ||
		TOMATOsEngine::IsKeyPressed(DIK_RIGHT) ||
		pad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ||
		pad.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {

		move.x = 1.0f;

	} else if (TOMATOsEngine::IsKeyPressed(DIK_A) ||
		TOMATOsEngine::IsKeyPressed(DIK_LEFT) ||
		pad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ||
		-pad.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {

		move.x = -1.0f;
	  }

	bool isJumpPressed = TOMATOsEngine::IsKeyTrigger(DIK_SPACE) || ((pad.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_B));

	float acceleration = isOnGround_ ? moveAcceleration_ : airAcceleration_;
	if (move.x != 0.0f) {
		velocity_.x += move.x * acceleration;
	}
	else if (isOnGround_) {
		velocity_.x *= friction_;
	}

	velocity_.x = std::clamp(velocity_.x, -maxMoveSpeed_, maxMoveSpeed_);

	if (isJumpPressed) {
		if (isOnGround_) {
			//地上ジャンプ
			velocity_.y = jumpPower_;
			isOnGround_ = false;
		}
		else if (isWallSliding_) {
			//壁キック
			velocity_.y = wallJumpPower_.y;
			velocity_.x = wallJumpPower_.x * -wallDirection_;
		}
	}

	velocity_.y += gravity_;

	if (isWallSliding_ && velocity_.y < wallSlideSpeed_) {
		velocity_.y = wallSlideSpeed_;
	}

	position_ += velocity_;

	CheckCollisions();
}

void Player::Draw() {
	TOMATOsEngine::DrawRectAngle(position_, size_, Vector2{0.5f,0.5f}, 0.0f,0xFFFFFFFF);
}

void Player::CheckCollisions()
{
	isOnGround_ = false;
	isWallSliding_ = false;
	wallDirection_ = 0;

	//地面
	if (position_.y <= size_.y / 2.0f) {
		position_.y = size_.y / 2.0f;
		if (velocity_.y < 0.0f) {
			velocity_.y = 0;
		}
		isOnGround_ = true;
	}


	//壁
	if (position_.x <= size_.x / 2.0f) {
		position_.x = size_.x / 2.0f;
		velocity_.x = 0;
		wallDirection_ = -1;
	}
	else if (position_.x >= TOMATOsEngine::kMonitorWidth - size_.x / 2.0f) {
		position_.x = TOMATOsEngine::kMonitorWidth - size_.x / 2.0f;
		velocity_.x = 0;
		wallDirection_ = 1;
	}

	if (wallDirection_ != 0 && !isOnGround_) {
		isWallSliding_ = true;
	}

}


