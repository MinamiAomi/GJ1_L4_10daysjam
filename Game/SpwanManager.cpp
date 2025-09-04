#include "SpwanManager.h"

#include <vector>  
#include <utility> 

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
	position_ = range_;
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
	int createCount = 8;

	SpawnBomb(createCount);
}

void SpawnManager::SpawnBomb(int createCount)
{
	auto& bombManager = StageObjectManager::GetInstance()->GetBombManager();

	Random::RandomNumberGenerator rnd{};
	//一時保管用
	std::vector<std::pair<Vector2, float>> spawnedBombs;

	// 1体あたりの位置決めの最大試行回数
	const int maxAttempts = 10;

	for (int i = 0; i < createCount; i++) {
		Vector2 pos;
		const float radius = 1.0f;
		bool isOverlapping;
		int attempts = 0;

		// 重ならない位置が見つかるか、試行回数が上限に達するまでループ
		do {
			pos.x = rnd.NextFloatRange(position_ + radius, position_ + range_);
			pos.y = rnd.NextFloatRange(radius, Wall::GetInstance()->kWallHeight);

			// 今まで生成したものと重なっているかどうか
			isOverlapping = false;
			for (const auto& spawned : spawnedBombs) {
				const Vector2& spawnedPos = spawned.first;
				const float spawnedRadius = spawned.second;

				//円の当たり判定
				float dx = pos.x - spawnedPos.x;
				float dy = pos.y - spawnedPos.y;
				float distance = (dx * dx) + (dy * dy);

				float sumRadius = radius + spawnedRadius;

				//重なっているか
				if (distance < (sumRadius * sumRadius)) {
					isOverlapping = true;
					break;
				}
			}

			attempts++;

		} while (isOverlapping && attempts < maxAttempts);

		// 重ならない位置が見つかった場合生成
		if (!isOverlapping) {
			bombManager.Spawn(pos, radius, 0xff1919FF);
			spawnedBombs.push_back({ pos, radius });
		}
	}
}

int SpawnManager::CalculationCreateCount()
{
	const auto& wallPos = Wall::GetInstance()->GetPosition();

	//距離を100で割ってます
	float createCount = (wallPos + 200.0f) / 100.0f;

	//敵個数Max10
	createCount = std::clamp(createCount, 0.0f, 10.0f);
	return int(createCount);
}
