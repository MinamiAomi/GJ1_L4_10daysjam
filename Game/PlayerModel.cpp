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
	rightFoot_ = Square{ {head_.size.x / 2.0f,-head_.size.y / 2.0f},player_->GetSize() / 3.0f };
} 

void PlayerModel::Update() {
	
}

void PlayerModel::Draw() {
	TOMATOsEngine::DrawBoxLine3D(head_ + player_->GetPosition(), 0xFFFFFFFF);
	TOMATOsEngine::DrawBoxLine3D(leftFoot_ + player_->GetPosition(), 0xFFFFFFFF);
	TOMATOsEngine::DrawBoxLine3D(rightFoot_ + player_->GetPosition(), 0xFFFFFFFF);
}


