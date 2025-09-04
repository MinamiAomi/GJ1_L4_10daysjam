#pragma once

class SpawnManager {
public:
	static SpawnManager* GetInstance();

	void Initialize();

	void Update();
private:
	void Spawn();

	float range_;
	float position_;
};