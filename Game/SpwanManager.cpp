#include "SpwanManager.h"

#include <numeric>   
#include <string>    

#include "TOMATOsEngine.h"

#include "Wall.h"
#include "StageObjectManager.h"
#include "Math/Random.h"

namespace {
	constexpr float kDefaultSpawnAreaWidth = 50.0f;
	constexpr float kDefaultSpawnOffsetY = 10.0f;
	constexpr int kDefaultTotalCreateCount = 15;
	constexpr int kMaxPlacementAttempts = 10;
}

SpawnManager* SpawnManager::GetInstance() {
	static SpawnManager instance;
	return &instance;
}

void SpawnManager::Initialize() {
	spawnAreaWidth_ = kDefaultSpawnAreaWidth;
	currentSpawnAreaStartX_ = spawnAreaWidth_;
	spawnOffsetY_ = kDefaultSpawnOffsetY;
	totalCreateCount_ = kDefaultTotalCreateCount;

	//出てくるボムの設定
	bombSpecs_ = {
		{ BombsType::Normal, 2.0f, 0xff1919FF, 1.0f },
		{ BombsType::Hit,    2.0f, 0x00E6E6FF, 1.0f },
	};

	// 重みの合計を計算
	totalBombSpawnWeight_ = 0.0f;
	for (const auto& spec : bombSpecs_) {
		totalBombSpawnWeight_ += spec.spawnWeight;
	}

	//高さの分割
	spawnHeightDivisions_ = 3;
	// [低、中、高] 
	spawnHeightWeights_ = { 0.3f, 0.6f, 0.2f };
	// 重みの合計を計算
	totalHeightSpawnWeight_ = std::accumulate(spawnHeightWeights_.begin(), spawnHeightWeights_.end(), 0.0f);
	//最初の生成
	FirstSpawn();
}

void SpawnManager::Update() {
#ifdef _DEBUG
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("Spawn")) {
		ImGui::DragInt("TotalCreateCount", &totalCreateCount_, 1, 0, 100);
		ImGui::Separator();

		ImGui::Text("BombSpawnWeights");
		for (auto& spec : bombSpecs_) {
			std::string label;
			switch (spec.type) {
			case BombsType::Normal:
				label = "NormalBomb";
				break;
			case BombsType::Hit:
				label = "HitBomb";
				break;
			default:
				label = "UnknownBomb";
				break;
			}
			std::string unique_label = label + " Weight##" + std::to_string(static_cast<int>(spec.type));
			ImGui::DragFloat(unique_label.c_str(), &spec.spawnWeight, 0.05f, 0.0f, 10.0f);
		}
		// 重みの合計を再計算
		totalBombSpawnWeight_ = 0.0f;
		for (const auto& spec : bombSpecs_) { totalBombSpawnWeight_ += spec.spawnWeight; }

		ImGui::Separator();
		ImGui::Text("HeightSpawnWeights");
		// 高さの重みの変更
		for (size_t i = 0; i < spawnHeightWeights_.size(); ++i) {
			std::string label = "HeightZone " + std::to_string(i) + " Weight##" + std::to_string(i);
			ImGui::DragFloat(label.c_str(), &spawnHeightWeights_[i], 0.05f, 0.0f, 10.0f);
		}
		// 重みの合計を再計算
		totalHeightSpawnWeight_ = std::accumulate(spawnHeightWeights_.begin(), spawnHeightWeights_.end(), 0.0f);

		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

	// スポーン領域を次に進めるかどうかの判定
	const float wallPos = Wall::GetInstance()->GetPosition();
	if (currentSpawnAreaStartX_ + spawnAreaWidth_ <= wallPos + spawnAreaWidth_ * 3.0f) {
		ProcessSpawning();
		currentSpawnAreaStartX_ += spawnAreaWidth_;
	}
}

void SpawnManager::ProcessSpawning() {
	// このスポーン処理内で生成されたボムの情報を一時的に保持する
	std::vector<std::pair<Vector2, float>> spawnedBombs;

	for (int i = 0; i < totalCreateCount_; ++i) {
		// どのボムが出るか
		const BombInfo& selectedSpec = SelectRandomBombType();

		// ボム生成
		TrySpawnSingleBomb(selectedSpec, spawnedBombs);
	}
}

void SpawnManager::FirstSpawn()
{
	// このスポーン処理内で生成されたボムの情報を一時的に保持する
	std::vector<std::pair<Vector2, float>> spawnedBombs;

	for (int i = 0; i < totalCreateCount_; ++i) {
		// ボム生成
		TrySpawnSingleBomb(bombSpecs_.at(0), spawnedBombs);
	}

	currentSpawnAreaStartX_ += spawnAreaWidth_;
}

const BombInfo& SpawnManager::SelectRandomBombType() const {
	Random::RandomNumberGenerator rnd{};
	// 重みの中でランダム数字
	float randomValue = rnd.NextFloatRange(0.0f, totalBombSpawnWeight_);

	// 抽選
	float accumulatedWeight = 0.0f;
	for (const auto& spec : bombSpecs_) {
		accumulatedWeight += spec.spawnWeight;
		if (randomValue <= accumulatedWeight) {
			return spec;
		}
	}
	// 最悪一番最初を返す
	return bombSpecs_.front();
}

void SpawnManager::TrySpawnSingleBomb(const BombInfo& spec, std::vector<std::pair<Vector2, float>>& existingBombs) {
	Vector2 spawnPos;
	// 重複しない座標を探す
	bool foundPosition = FindNonOverlappingPosition(spec.radius, existingBombs, spawnPos);

	if (foundPosition) {
		auto& bombManager = StageObjectManager::GetInstance()->GetBombManager();
		//ボム生成
		switch (spec.type) {
		case BombsType::Normal:
			bombManager.SpawnBomb(spawnPos, spec.radius, spec.color);
			break;
		case BombsType::Hit:
			bombManager.SpawnHitBomb(spawnPos, spec.radius, spec.color);
			break;
		}
		//リストに追加
		existingBombs.push_back({ spawnPos, spec.radius });
	}
}

float SpawnManager::SelectRandomSpawnY() const {
	Random::RandomNumberGenerator rnd{};

	// どの高さの区画に出るか
	float randomValue = rnd.NextFloatRange(0.0f, totalHeightSpawnWeight_);
	float accumulatedWeight = 0.0f;
	int selectedDivision = 0;
	for (int i = 0; i < spawnHeightDivisions_; ++i) {
		accumulatedWeight += spawnHeightWeights_[i];
		if (randomValue <= accumulatedWeight) {
			selectedDivision = i;
			break;
		}
	}

	// 決定した区画内でランダムなY座標を計算
	const float totalSpawnableHeight = Wall::GetInstance()->kWallHeight - spawnOffsetY_;
	const float divisionHeight = totalSpawnableHeight / spawnHeightDivisions_;

	float minY = spawnOffsetY_ + divisionHeight * selectedDivision;
	float maxY = minY + divisionHeight;

	return rnd.NextFloatRange(minY, maxY);
}

bool SpawnManager::FindNonOverlappingPosition(
	float radius,
	const std::vector<std::pair<Vector2, float>>& existingBombs,
	Vector2& outPosition
) const {
	Random::RandomNumberGenerator rnd{};

	// 最大試行回数まで、重ならない位置を探し続ける
	for (int attempts = 0; attempts < kMaxPlacementAttempts; ++attempts) {
		Vector2 pos;
		// X座標はスポーン範囲内でランダム
		pos.x = rnd.NextFloatRange(currentSpawnAreaStartX_ + radius, currentSpawnAreaStartX_ + spawnAreaWidth_ - radius);
		// Y座標は確率に基づいて決定
		pos.y = SelectRandomSpawnY();

		// 他のボムと重なっているかチェック
		bool isOverlapping = false;
		for (const auto& spawned : existingBombs) {
			const Vector2& spawnedPos = spawned.first;
			const float spawnedRadius = spawned.second;

			// 円同士の当たり判定
			float dx = pos.x - spawnedPos.x;
			float dy = pos.y - spawnedPos.y;
			float distanceSquared = (dx * dx) + (dy * dy);

			float sumRadius = radius + spawnedRadius;
			float sumRadiusSquared = sumRadius * sumRadius;

			//重なっていたらやめとく
			if (distanceSquared < sumRadiusSquared) {
				isOverlapping = true;
				break; 
			}
		}

		// 重なっていなければ、その位置を採用して成功を返す
		if (!isOverlapping) {
			outPosition = pos;
			return true;
		}
	}

	// 最大試行回数に達しても見つからなければ失敗を返す
	return false;
}