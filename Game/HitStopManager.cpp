#include "HitStopManager.h"

#include <algorithm>

#include "TOMATOsEngine.h"

HitStopManager* HitStopManager::GetInstance()
{
	static HitStopManager instance;
	return &instance;
}

void HitStopManager::Initialize()
{
	isHitStop_ = false;
	time_ = 0;
	maxTime_ = 3;
}

void HitStopManager::Update()
{
#ifdef _DEBUG
	ImGui::Begin("InGame");
	if (ImGui::BeginMenu("HitStopManager")) {
		ImGui::DragInt("MaxTime", &maxTime_, 1, 0, 60);
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
	if (time_ <= 0) {
		isHitStop_ = false;
	}
	else {
		time_--;
	}
	time_ = std::clamp(time_, 0, maxTime_);
}

void HitStopManager::SetIsHitStop()
{
	isHitStop_ = true;
	time_ = maxTime_;
}
