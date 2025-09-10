// SpawnManager.h
#pragma once

#include <vector>
#include <cstdint> 
#include "Math/MathUtils.h"

enum class BombsType {
    Normal,
    Hit,
};

struct BombInfo {
    BombsType type = BombsType::Normal;
    float radius = 2.0f;
    uint32_t color = 0xFFFFFFFF;
    //大きいほど出やすい
    float spawnWeight = 1.0f; 
};

class SpawnManager {
public:
    static SpawnManager* GetInstance();

    void Initialize();
    void Update();

private:
    SpawnManager() = default;
    ~SpawnManager() = default;
    SpawnManager(const SpawnManager&) = delete;
    SpawnManager& operator=(const SpawnManager&) = delete;

    void ProcessSpawning();

    void FirstSpawn();

    // 確率に基づいて次にスポーンするボムの仕様を選択する
    const BombInfo& SelectRandomBombType() const;

    // 確率に基づいてスポーンするY座標を決定する
    float SelectRandomSpawnY() const;

    // 1つのボムを、重ならないように配置を試みる
    void TrySpawnSingleBomb(
        const BombInfo& spec,
        std::vector<std::pair<Vector2, float>>& existingBombs
    );

    // スポーンさせるための位置をランダムに決定し、重複がないかチェックする
    bool FindNonOverlappingPosition(
        float radius,
        const std::vector<std::pair<Vector2, float>>& existingBombs,
        Vector2& outPosition
    ) const;


    // スポーン範囲の幅
    float spawnAreaWidth_;
    // 現在のスポーン範囲の開始X座標
    float currentSpawnAreaStartX_;
    // ボムをどの高さからスポーンさせるか（Y座標のオフセット）
    float spawnOffsetY_;

    // 一度に生成するボムの総数
    int totalCreateCount_;


    // ボムの仕様リスト(ここに突っ込むとスポーン対象になる)
    std::vector<BombInfo> bombSpecs_;

    // スポーンする高さを何分割するか
    int spawnHeightDivisions_;
    // 分割された各高さ区画でのスポーン確率の「重み」
    std::vector<float> spawnHeightWeights_;

    // 内部計算用の変数
    float totalBombSpawnWeight_ = 0.0f;
    float totalHeightSpawnWeight_ = 0.0f;
};