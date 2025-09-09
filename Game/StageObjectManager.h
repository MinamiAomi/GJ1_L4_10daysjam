#pragma once

#include "BombManager.h"

class  StageObjectManager {
public:
	static StageObjectManager* GetInstance();

	void Initialize();

	void Update();

	void Draw();

	void Despawn();

	BombManager& GetBombManager(){ return bombManager_; }
private:
	BombManager bombManager_;

};