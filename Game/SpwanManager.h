#pragma once

class SpawnManager {
public:
	static SpawnManager* GetInstance();

	void Initialize();

	void Update();
private:
	void Spawn();

	void SpawnBomb(int createCount);

	int CalculationCreateCount();

	float range_;
	float position_;
};