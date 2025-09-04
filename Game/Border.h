#pragma once

#include "Math/MathUtils.h"


class Border {
public:
	const float kWallWidth = 3.0f;
	const float kWallHeight = 50.0f;

	static Border* GetInstance();

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

	bool IsMove() { return pushBackPosition_ != 0.0f; }
	//現在の横BorderLinePos
	float GetBorderSidePos();
	/// <summary>
	/// ゲームスタート時のPosition(俺以外使わないと思う)
	/// </summary>
	/// <returns></returns>
	float GetBorderFirstPos();
private:
	//手前のライン
	float position_;
	//押し戻しのポジション
	float pushBackPosition_;
	//イージングの速度
	float easingSpeed_;
	//押し戻し係数
	float pushBackCoefficient_;

	int color_;
	float firstPosition_;
};