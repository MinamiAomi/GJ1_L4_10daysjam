#include "HitBomb.h"

#include "TOMATOsEngine.h"
#include "HitStopManager.h"

#include "Player.h"
#include "CollisionManager.h"
#include "Particle/ParticleManager.h"
#include "Wall.h"
#include "Border.h"

#include "Math/Color.h"

void HitBomb::Initialize(const Vector2& position, float radius, int color)
{
	position_ = position;
	radius_ = radius;
	color_ = color;
	isAlive_ = true;
}

void HitBomb::Update()
{
	const auto& wall = Wall::GetInstance();
	//wallに触れたら
	if (position_.x - radius_ < wall->GetPosition()) {
		position_.x = wall->GetPosition() + radius_;
	}
}

void HitBomb::Draw()
{
	TOMATOsEngine::DrawBoxLine3D(position_, { radius_,radius_ }, color_);
}

void HitBomb::Despawn()
{
	ParticleManager::GetInstance()->GetPop()->Create(position_, Color::Convert(color_), 10);
}

void HitBomb::OnCollision(const Vector2& position, float radius)
{
	//自分が当たってるやつより左にいるときだけ押し戻し
	if (position_.x > position.x) {
		position_.x = position.x + radius;
	}
}

void HitBomb::OnPlayerHitCollision(Player* player)
{
	const auto& border = Border::GetInstance();
	const auto& particle = ParticleManager::GetInstance();

	isAlive_ = false;

	//ヒップドロップしているかどうか
	if (player->GetIsHipDrop()) {
		border->PushBack(1);
		//スコア表示パーティクル
		particle->GetNumber()->Create(position_, Color(Color::white), int(border->GetPushBackScore()));
		//ヒットストップ
		HitStopManager::GetInstance()->SetIsHitStop();
	}
	else {
		border->PushBack(-1);
		//スコア表示パーティクル
		particle->GetNumber()->Create(position_, Color::Convert(color_), int(border->GetPushBackScore()));
	}
	//死亡時飛び散りパーティクル
	particle->GetPop()->Create(position_, Color::Convert(color_), 10);

}