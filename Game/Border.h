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

	//現在のBorderLinePos
	float BorderLinePos() {
		return position_.x - size_.x;
	}
private:
	Vector2 position_;
	Vector2 size_;
	//borderが迫る速度
	float velocity_;
	//押し戻しのポジション
	float pushBackPosition_;
	//イージングの速度
	float easingSpeed_;
	//押し戻し係数
	float pushBackCoefficient_;

};