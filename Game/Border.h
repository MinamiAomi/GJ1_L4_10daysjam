#pragma once

#include "Math/MathUtils.h"


class Border {
public:
	void Initialize();

	void Update();

	void Draw();

	/// <summary>
	/// 0.5f->ヒップドロップ
	/// 1.0ff->1爆弾
	/// 2.0ff->2爆弾
	/// </summary>
	/// <param name="add"></param>
	void PushBack(float add);
private:
	Vector2 position;
	//borderが迫る速度
	float velocity;
	//押し戻しのポジション
	float pushBackPosition;
	//イージングの速度
	float easingSpeed;
	//押し戻し係数
	float pushBackCoefficient;
};