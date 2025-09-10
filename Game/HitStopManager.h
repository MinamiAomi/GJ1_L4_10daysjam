#pragma once

class HitStopManager {
public:
	static HitStopManager* GetInstance();

    void Initialize();

    void Update();

    bool GetIsHitStop() { return isHitStop_; }
    void SetIsHitStop();
private:
    HitStopManager() = default;
    ~HitStopManager() = default;
    HitStopManager(const HitStopManager&) = delete;
    HitStopManager& operator=(const HitStopManager&) = delete;

    bool isHitStop_;
    int time_;
    int maxTime_;
};