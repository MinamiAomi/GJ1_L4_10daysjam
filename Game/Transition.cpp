#include "Transition.h"
#include "Math/MathUtils.h"
#include "TOMATOsEngine.h"

Transition* Transition::GetInstance()
{
	static Transition instance;
	return &instance;
}


void Transition::Update()
{
	if (isStart) {
		isNextSceneFrame = false;
		if (t < 1.0f) {
			t += speed;
			t = std::clamp(t, 0.0f, 1.0f);
			TOMATOsEngine::SetLineShakeX(true, t * maxShakeValue);
			TOMATOsEngine::SetLineShakeY(true, t * maxShakeValue);
			isPreScene = true;
		}
		if (t >= 1.0f && t2 <= 0.0f) {
			isNextSceneFrame = true;
		}
		if (t >= 1.0f) {
			t2 += speed;
			t2 = std::clamp(t, 0.0f, 1.0f);
			TOMATOsEngine::SetLineShakeX(true, t2 * maxShakeValue);
			TOMATOsEngine::SetLineShakeY(true, t2 * maxShakeValue);
			isNextScene = true;
		}
		if (t2 >= 1.0f) {
			isStart = false;
			isPreScene = false;
			isNextScene = false;
			t = 0.0f;
			t2 = 0.0f;
		}
	}
}
