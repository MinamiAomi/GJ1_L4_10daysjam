#include "SpwanManager.h"

#include "Math/Random.h"

#include "Border.h"
#include "Wall.h"
#include "StageObjectManager.h"

SpawnManager* SpawnManager::GetInstance()
{
	static SpawnManager instance;
	return &instance;
}

void SpawnManager::Initialize()
{
	position_ = 0.0f;
	range_ = 50.0f;

}

void SpawnManager::Update()
{
	//カメラ外にもスポーンさせるため
	const auto& wallPos = Wall::GetInstance()->GetPosition();
	if (position_ + range_ <= wallPos + range_ * 3.0f) {
		Spawn();
		//範囲を次に
		position_ += range_;
	}
}

void SpawnManager::Spawn()
{
	Random::RandomNumberGenerator rnd{};

	auto& bombManager = StageObjectManager::GetInstance()->GetBombManager();

	//いったん固定で5体
	for (int i = 0; i < 5; i++) {
		Vector2 pos;
		float radius = 2.0f;
		pos.x = rnd.NextFloatRange(position_ + radius, position_ + range_);
		pos.y = rnd.NextFloatRange(radius, Wall::GetInstance()->kWallHeight);

		bombManager.Spawn(pos,radius,0xFF0000FF);
	}

}
