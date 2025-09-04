#include "PlayerModel.h"

#include "TOMATOsEngine.h"
#include "Math/Color.h"
#include "Particle/ParticleManager.h"
#include "Math/Random.h"

#include "Easing.h"
#include "Player.h"

void PlayerModel::Initialize(const Player* player) {
	player_ = player;
	head_ = Square{ {0.0f,0.0f},player_->GetSize() - player_->GetSize() / 3.0f};
	leftFoot_ = Square{ -head_.size / 2.0f,player_->GetSize() / 3.0f };
	initialLeftFootPos_ = leftFoot_.center;
	rightFoot_ = Square{ {head_.size.x / 2.0f,-head_.size.y / 2.0f},player_->GetSize() / 3.0f };
	initialRightFootPos_ = rightFoot_.center;
	walkAmplitude_ = rightFoot_.size.y / 2.0f;
	idleAmplitude_ = head_.size.y / 4.0f;
} 

void PlayerModel::Update() {

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
	default:
		break;
	}
}

void PlayerModel::Draw() {
	TOMATOsEngine::DrawBoxLine3D(head_ + player_->GetPosition(), 0xFFFFFFFF);
	TOMATOsEngine::DrawBoxLine3D(leftFoot_ + player_->GetPosition(), 0xFFFFFFFF);
	TOMATOsEngine::DrawBoxLine3D(rightFoot_ + player_->GetPosition(), 0xFFFFFFFF);
}

void PlayerModel::AnimationInitialize()
{
	walkCycle_ = 0.0f;
	idleCycle_ = 0.0f;
}

void PlayerModel::Idle()
{

	leftFoot_.center = initialLeftFootPos_ ;
	rightFoot_.center = initialRightFootPos_;
}

void PlayerModel::Move()
{
	walkCycle_ += walkSpeed_;
	float yOffset = sin(walkCycle_) * walkAmplitude_;

	leftFoot_.center.y = initialLeftFootPos_.y + yOffset;
	rightFoot_.center.y = initialLeftFootPos_.y - yOffset;
}

void PlayerModel::Jump()
{
}


