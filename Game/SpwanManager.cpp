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

	bombRadius_ = 2.0f;
	offset_ = 10.0f;
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
	int createCount = 6;

	SpawnBomb(createCount);
	SpawnHitBomb(createCount);
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
		bool isOverlapping;
		int attempts = 0;

		// 重ならない位置が見つかるか、試行回数が上限に達するまでループ
		do {
			pos.x = rnd.NextFloatRange(position_ + bombRadius_, position_ + range_);
			pos.y = rnd.NextFloatRange(offset_ + bombRadius_, Wall::GetInstance()->kWallHeight);

			// 今まで生成したものと重なっているかどうか
			isOverlapping = false;
			for (const auto& spawned : spawnedBombs) {
				const Vector2& spawnedPos = spawned.first;
				const float spawnedRadius = spawned.second;

				//円の当たり判定
				float dx = pos.x - spawnedPos.x;
				float dy = pos.y - spawnedPos.y;
				float distance = (dx * dx) + (dy * dy);

				float sumRadius = bombRadius_ + spawnedRadius;

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
			bombManager.SpawnBomb(pos, bombRadius_, 0xff1919FF);
			spawnedBombs.push_back({ pos, bombRadius_ });
		}
	}
}

void SpawnManager::SpawnHitBomb(int createCount)
{
	auto& bombManager = StageObjectManager::GetInstance()->GetBombManager();

	Random::RandomNumberGenerator rnd{};
	//一時保管用
	std::vector<std::pair<Vector2, float>> spawnedBombs;

	// 1体あたりの位置決めの最大試行回数
	const int maxAttempts = 10;

	for (int i = 0; i < createCount; i++) {
		Vector2 pos;
		bool isOverlapping;
		int attempts = 0;

		// 重ならない位置が見つかるか、試行回数が上限に達するまでループ
		do {
			pos.x = rnd.NextFloatRange(position_ + bombRadius_, position_ + range_);
			pos.y = rnd.NextFloatRange(offset_ + bombRadius_, Wall::GetInstance()->kWallHeight);

			// 今まで生成したものと重なっているかどうか
			isOverlapping = false;
			for (const auto& spawned : spawnedBombs) {
				const Vector2& spawnedPos = spawned.first;
				const float spawnedRadius = spawned.second;

				//円の当たり判定
				float dx = pos.x - spawnedPos.x;
				float dy = pos.y - spawnedPos.y;
				float distance = (dx * dx) + (dy * dy);

				float sumRadius = bombRadius_ + spawnedRadius;

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
			bombManager.SpawnHitBomb(pos, bombRadius_, 0x00E6E6FF);
			spawnedBombs.push_back({ pos, bombRadius_ });
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
