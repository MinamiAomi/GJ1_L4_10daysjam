#include "Bomb.h"

#include "TOMATOsEngine.h"

#include "Player.h"
#include "CollisionManager.h"
#include "Particle/ParticleManager.h"
#include "Wall.h"
#include "Border.h"

#include "Math/Color.h"

void Bomb::Initialize(const Vector2& position, float radius, int color)
{
	position_ = position;
	radius_ = radius;
	color_ = color;
	isAlive_ = true;
}

void Bomb::Update()
{
	const auto& wall = Wall::GetInstance();
	//wallに触れたら
	if (position_.x - radius_ < wall->GetPosition()) {
		position_.x = wall->GetPosition() + radius_;
	}
}

void Bomb::Draw()
{
	TOMATOsEngine::DrawBoxLine3D(position_, { radius_,radius_ }, color_);
}

void Bomb::OnCollision(const Vector2& position, float radius)
{
	//自分が当たってるやつより左にいるときだけ押し戻し
	if (position_.x > position.x) {
		position_.x = position.x + radius;
	}
}

void Bomb::OnPlayerHitCollision(Player* player)
{
	player;
	const auto& border = Border::GetInstance();
	const auto& particle = ParticleManager::GetInstance();

	isAlive_ = false;

	border->PushBack(1);

	//ヒップドロップしているかどうか 
	if (player->GetIsHipDrop()) {
		//スコア表示パーティクル
		particle->GetNumber()->Create(position_, Color(Color::white), int(border->GetPushBackScore()));
	}
	else {
		particle->GetNumber()->Create(position_, Color(color_), int(border->GetPushBackScore()));
	}
	particle->GetPop()->Create(position_, Color::Convert(color_), 10);
}