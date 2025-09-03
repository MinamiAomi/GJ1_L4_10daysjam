#include "StageObjectManager.h"

StageObjectManager* StageObjectManager::GetInstance()
{
	static StageObjectManager instance;
	return &instance;
}

void StageObjectManager::Initialize()
{
	bombManager_.Initialize();
}

void StageObjectManager::Update()
{
	bombManager_.Update();
}

void StageObjectManager::Draw()
{
	bombManager_.Draw();
}