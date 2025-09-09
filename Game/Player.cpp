#include "Player.h"

#include "TOMATOsEngine.h"
#include "Math/Color.h"
#include "Particle/ParticleManager.h"
#include "Math/Random.h"
#include "Border.h"
#include "Wall.h"

#include "Easing.h"

void Player::Initialize() {
	position_ = Vector2::zero;
	velocity_ = Vector2::zero;
	size_ = { 3.0f, 3.0f };
	playerModel_.Initialize(this);
	isHipDrop_ = false;
	isFacing = true;
	rotate_ = 0.0f;
	playerParticleColor_ = {1.0f,1.0f,1.0f,1.0f};
}

void Player::Update() {
#ifdef _DEBUG
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("Player")) {
		ImGui::Text("Position:x%f,y:%f", position_.x, position_.y);
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG


	const auto& pad = TOMATOsEngine::GetGamePadState();
	const auto prepad = TOMATOsEngine::GetGamePadPreState();

	playerModel_.SetState(PlayerModel::kIdle);


	//HipDrop
	if (!isHipDrop_ && (TOMATOsEngine::IsKeyTrigger(DIK_LSHIFT) || TOMATOsEngine::IsKeyTrigger(DIK_S) ||
		((pad.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(prepad.Gamepad.wButtons & XINPUT_GAMEPAD_A)) && (!isOnGround_ && !isWallSliding_))) {
		isHipDrop_ = true;
	}

	if (!isHipDrop_) {
		Move();
		if (!isOnGround_) {
			playerModel_.SetState(PlayerModel::kJump);
		}
	}
	else {
		playerModel_.SetState(PlayerModel::kHipDrop);
		HipDrop();
	}


	position_ += velocity_;

	CheckCollisions();

	playerModel_.Update();
}

void Player::Draw() {
	playerModel_.Draw();
}

void Player::Move() {

	Vector2 move = Vector2::zero;
	const auto& pad = TOMATOsEngine::GetGamePadState();
	const auto prepad = TOMATOsEngine::GetGamePadPreState();

	if (TOMATOsEngine::IsKeyPressed(DIK_D) ||
		TOMATOsEngine::IsKeyPressed(DIK_RIGHT) ||
		pad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ||
		pad.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {

		move.x = 1.0f;
		isFacing = true;
		playerModel_.SetState(PlayerModel::kMove);

	}
	else if (TOMATOsEngine::IsKeyPressed(DIK_A) ||
		TOMATOsEngine::IsKeyPressed(DIK_LEFT) ||
		pad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ||
		-pad.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {

		move.x = -1.0f;
		isFacing = false;
		playerModel_.SetState(PlayerModel::kMove);
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
			playerModel_.SetState(PlayerModel::kJump);
		}
		else if (isWallSliding_) {
			//壁キック
			velocity_.y = wallJumpPower_.y;
			velocity_.x = wallJumpPower_.x * -wallDirection_;
			Vector2 particlePos = { position_.x + (isFacing ? (size_.x / 2.0f) : (-size_.x / 2.0f)) ,position_.y };
			ParticleManager::GetInstance()->GetSplash()->Create(particlePos, { isFacing ? 1.0f : -1.0f ,0.0f }, playerParticleColor_, 8);
		}
	}

	velocity_.y += gravity_;

	if (isWallSliding_ && velocity_.y < wallSlideSpeed_) {
		velocity_.y = wallSlideSpeed_;
	}
}

void Player::HipDrop()
{
	velocity_.x = 0.0f;

	if (rotate_ < Math::TwoPi) {
		rotate_ += hipDropRotateSpeed_;
		velocity_.y = hipDropUpSpeed_;
	}
	else {
		velocity_.y = -hipDropSpeed_;
		rotate_ = Math::TwoPi;
	}

	if (isOnGround_) {
		Vector2 particlePos = { position_.x,position_.y + (-size_.y / 2.0f + 0.1f) };
		ParticleManager::GetInstance()->GetSplash()->Create(particlePos, { 0.0f,1.0f }, { 1.0f,1.0f,1.0,1.0f }, 8);
		playerModel_.SetState(PlayerModel::kEndHipDrop);
		isHipDrop_ = false;
		rotate_ = 0.0f;
	}
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
	if (position_.x <= Wall::GetInstance()->GetPosition() + size_.x / 2.0f) {
		position_.x = Wall::GetInstance()->GetPosition() + size_.x / 2.0f;
		wallDirection_ = -1;
		isFacing = true;
	}
	else if (position_.x >= Border::GetInstance()->GetBorderSidePos() - size_.x / 2.0f) {
		position_.x = Border::GetInstance()->GetBorderSidePos() - size_.x / 2.0f;
		wallDirection_ = 1;
		isFacing = false;
	}

	if (wallDirection_ != 0 && !isOnGround_) {
		isWallSliding_ = true;
		playerModel_.SetState(PlayerModel::kWallSliding);
	}

}


