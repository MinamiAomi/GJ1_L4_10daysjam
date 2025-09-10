#include "PlayerModel.h"
#include "ImGuiManager.h"

#include "TOMATOsEngine.h"
#include "Math/Color.h"
#include "Particle/ParticleManager.h"
#include "Math/Random.h"

#include "Easing.h"
#include "Player.h"
#include "Wall.h"

void PlayerModel::Initialize(const Player* player) {
	player_ = player;
	head_ = Square{ {0.0f,0.0f},player_->GetSize() - player_->GetSize() / 3.0f};
	initialHead_ = head_;
	leftFoot_ = Square{ -head_.size / 2.0f,player_->GetSize() / 3.0f };
	initialLeftFoot_ = leftFoot_;
	rightFoot_ = Square{ {head_.size.x / 2.0f,-head_.size.y / 2.0f},player_->GetSize() / 3.0f };
	initialRightFoot_ = rightFoot_;
	walkAmplitude_ = rightFoot_.size.y / 2.0f;
	idleAmplitude_ = head_.size.y / 6.0f;
	changeT_ = 0.0f;
	jumpHead_ = Square{ head_.center,{head_.size.x - head_.size.x / 8.0f,head_.size.y + head_.size.y / 8.0f } };
	endHipDropSquare_ = Square{ {0.0f, -player_->GetSize().y / 2.0f + 0.1f}  ,{head_.size.x * 6.0f,0.1f} };

} 

void PlayerModel::Update() {


	if (state_ != preState_) {
		changeT_ = 0.0f;
	}

	switch (state_)
	{
	case PlayerModel::kIdle:
		Idle();
		break;
	case PlayerModel::kMove:
		Move();
		break;
	case PlayerModel::kJump:
		Jump();
		break;
	case PlayerModel::kWallSliding:
		WallSliding();
		break;
	case PlayerModel::kHipDrop:
		HipDrop();
		break;
	case PlayerModel::kEndHipDrop:
		EndHipDrop();
		break;
	default:
		break;
	}
	
	preState_ = state_;
	changeT_ = std::clamp(changeT_, 0.0f, 1.0f);
}

void PlayerModel::Draw() {
	TOMATOsEngine::DrawBoxLine3D(head_ + player_->GetPosition(), 0xFFFFFFFF);
	TOMATOsEngine::DrawBoxLine3D(leftFoot_ + player_->GetPosition(), 0xFFFFFFFF);
	TOMATOsEngine::DrawBoxLine3D(rightFoot_ + player_->GetPosition(), 0xFFFFFFFF);
	offsetY = Easing::easing(player_->GetWallToPosition().x / 90.0f, 0.1f, 0.2f,Easing::easeOutCubic);
	if (player_->GetPosition().y - player_->GetSize().y >= Wall::GetInstance()->kWallHeight + player_->GetWallToPosition().x * offsetY) {
		float y = player_->GetWallToPosition().x * offsetY;
		TOMATOsEngine::DrawArrow2D({ player_->GetPosition().x,Wall::GetInstance()->kWallHeight - 1.0f + y },3.0f,2.0f, 90.0f * Math::ToRadian,0xFFFFFFFF );
	}
}

void PlayerModel::ResultDraw() {
	state_ = kIdle;
	Vector2 offset = { 4.0f,2.0f };
	TOMATOsEngine::DrawBoxLine3D(head_  + offset, 0xFFFFFFFF);
	TOMATOsEngine::DrawBoxLine3D(leftFoot_  + offset, 0xFFFFFFFF);
	TOMATOsEngine::DrawBoxLine3D(rightFoot_  + offset, 0xFFFFFFFF);
}

void PlayerModel::Idle()
{
	changeT_ += 0.1f;

	leftFoot_ = Square::Lerp(changeT_, leftFoot_,initialLeftFoot_);
	rightFoot_ = Square::Lerp(changeT_, rightFoot_, initialRightFoot_);
	head_ = Square::Lerp(changeT_, head_, initialHead_);
	if (changeT_ >= 1.0f) {
		idleCycle_ += idleSpeed_;
		float yOffset = sin(idleCycle_) * idleAmplitude_;

		head_.center.y = initialHead_.center.y + yOffset;
	}
}

void PlayerModel::Move()
{
	changeT_ += 0.1f;

	leftFoot_ = Square::Lerp(changeT_, leftFoot_, initialLeftFoot_);
	rightFoot_ = Square::Lerp(changeT_, rightFoot_, initialRightFoot_);
	head_ = Square::Lerp(changeT_, head_, initialHead_);
	
	walkCycle_ += walkSpeed_;
	float yOffset = sin(walkCycle_) * walkAmplitude_;

	leftFoot_.center.y = initialLeftFoot_.center.y + yOffset;
	rightFoot_.center.y = initialLeftFoot_.center.y - yOffset;
}

void PlayerModel::Jump()
{
	changeT_ += 0.2f;
	head_ = Square::Lerp(changeT_, head_, jumpHead_);
	leftFoot_ = Square::Lerp(changeT_, leftFoot_, initialLeftFoot_);
	rightFoot_ = Square::Lerp(changeT_, rightFoot_, initialRightFoot_);
	if (player_->GetVelocity().x != 0.0f) {
		walkCycle_ += walkSpeed_;
		float yOffset = sin(walkCycle_) * walkAmplitude_;

		leftFoot_.center.y = initialLeftFoot_.center.y + yOffset;
		rightFoot_.center.y = initialLeftFoot_.center.y - yOffset;
	}
}

void PlayerModel::WallSliding()
{
	changeT_ += 0.2f;
	head_ = Square::Lerp(changeT_, head_, jumpHead_);
	leftFoot_.size = Vector2::Lerp(changeT_, leftFoot_.size, initialLeftFoot_.size);
	rightFoot_.size = Vector2::Lerp(changeT_, rightFoot_.size, initialRightFoot_.size);
	if (player_->GetFacing()) {
		//右
		leftFoot_.center.y = initialLeftFoot_.center.y + walkAmplitude_;
		leftFoot_.center.x = initialLeftFoot_.center.x;
		rightFoot_.center.y = initialRightFoot_.center.y - walkAmplitude_;
		rightFoot_.center.x = initialRightFoot_.center.x;
	}
	else {
		//左
		leftFoot_.center.y = initialLeftFoot_.center.y - walkAmplitude_;
		leftFoot_.center.x = initialLeftFoot_.center.x;
		rightFoot_.center.y = initialRightFoot_.center.y + walkAmplitude_;
		rightFoot_.center.x = initialRightFoot_.center.x;
	}
}

void PlayerModel::HipDrop()
{
	changeT_ += 0.1f;
	head_ = Square::Lerp(changeT_, head_, jumpHead_);
	head_.radian = player_->GetRotate();
	leftFoot_.center = initialLeftFoot_.center * Matrix3x3::MakeRotation(player_->GetRotate());
	rightFoot_.center = initialRightFoot_.center * Matrix3x3::MakeRotation(player_->GetRotate());
}

void PlayerModel::EndHipDrop()
{
	head_ = endHipDropSquare_;
	leftFoot_ = endHipDropSquare_;
	rightFoot_ = endHipDropSquare_;
}


