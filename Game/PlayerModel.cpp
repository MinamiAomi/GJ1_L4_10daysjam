#include "PlayerModel.h"

#include "TOMATOsEngine.h"
#include "Math/Color.h"
#include "Particle/ParticleManager.h"
#include "Math/Random.h"

#include "Easing.h"
#include "Player.h"

void PlayerModel::Initialize(const Player* player) {
	player_ = player;
} 

void PlayerModel::Update() {
	
}

void PlayerModel::Draw() {
	TOMATOsEngine::DrawLine3D(player_->GetPosition() - player_->GetSize() / 2.0f, player_->GetPosition() + player_->GetSize() / 2.0f, 0xFFFFFFFF);
}


