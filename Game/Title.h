#pragma once

#include <vector>

#include "Math/MathUtils.h"

class Title {
public:
	const float kAnimationDuration = 2.0f;
	void Initialize();
	void Update();
	void Draw();
private:
	enum State {
		//一番最初のアニメーション
		first,
		//タイトル画面中
		title,
		//インゲームに移行
		ingame,
	}state_ = first;

	//アニメーション管理
	float animationTime_;

	//タイトルの文字
	class TitleText {
	public:
		const float kScale = 10.0f;
		const float kSpace = 10.0f;
		const float kOutlineThickness = 0.1f;

		void Initialize();
		void Update(float animationTime);
		void Draw();
	private:
		Vector2 position_;

		//文字の大きさ
		float scale_;
		//文字の間隔
		float space_;
		//アウトラインの幅
		float outlineThickness_;
		//色
		Vector4 color_;


		const std::vector<std::vector<Vector2>> titleText_ = {
			//チ
			{
				{0.33f, 0.37f}, {-0.41f, 0.2f},
				{-0.5f, 0.04f}, {0.5f, 0.04f},
				{0.08f, 0.30f}, {0.08f, -0.23f},
				{0.08f, -0.23f}, {-0.33f, -0.37f},
			},
			//ル
			{
				{-0.25f, 0.33f}, {-0.33f, -0.41f},
				{0.0f, 0.41f}, {0.0f, -0.25f},
				{0.00f, -0.29f}, {0.5f, 0.08f},
			},
			//キ
			{
				{-0.41f, 0.2f}, {0.33f, 0.29f},
				{-0.41f, -0.04f}, {0.41f, 0.04f},
				{-0.08f, 0.5f}, {0.08f, -0.41f},
			},
			//っ
			{
				{-0.16f, 0.16f}, {0.08f, -0.08f},
				{-0.33f, 0.08f}, {-0.08f, -0.16f},
				{-0.33f, -0.41f}, {0.33f, -0.1f},
			},
			//ク
			{
				{-0.25f, 0.29f}, {-0.33f, -0.08f},
				{-0.25f, 0.29f}, {0.33f, 0.29f},
				{0.33f, 0.29f}, {0.33f, -0.08f},
				{0.33f, -0.08f}, {-0.25f, -0.41f}
			},
		};
	}titleText_;
};