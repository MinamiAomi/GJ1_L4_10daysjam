#include "Particle/Pop.h"

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"

void Pop::Initialize() {
	emitter_ = { 0.0f, 0.0f };
	for (auto& particle : particles_) {
		particle = std::make_unique<Particle>();
	}
}

void Pop::Create(const Vector2 emitter, Vector4 color, uint32_t MaxParticle) {
	Random::RandomNumberGenerator rnd{};
	const uint32_t deathtime_Min = 20;
	const uint32_t deathtime_Max = 30;
	//const float radius = 0.5f;
	const float size_Min = 0.8f;
	const float size_Max = 1.5f;
	const float speed_Min = 0.9f;
	const float speed_Max = 1.2f;

	emitter_ = emitter;

	if (MaxParticle == 0) {
		return;
	}

	const float angleStep = Math::TwoPi / static_cast<float>(MaxParticle);
	const float randomAngleMarginDegrees = 15.0f;
	const float randomAngleMarginRadians = randomAngleMarginDegrees * Math::ToRadian;

	// 全体の初期角度をランダム化する
	const float baseRotation = rnd.NextFloatRange(0.0f, Math::TwoPi);

	uint32_t count = 0;
	for (auto& particle : particles_) {
		if (count >= MaxParticle) {
			break;
		}

		if (!particle->isAlive_) {
			// このパーティクルの分割された角度
			float dividedAngle = static_cast<float>(count) * angleStep;

			// 個別のランダムなブレ
			float randomOffset = rnd.NextFloatRange(-randomAngleMarginRadians, randomAngleMarginRadians);

			// 計算した「ベース回転」を最終的な角度に加える 
			float finalAngle = baseRotation + dividedAngle + randomOffset;

			// 最終的な角度から方向ベクトルを生成
			particle->direction_ = { std::cosf(finalAngle), std::sinf(finalAngle) };


			// 速度・サイズ・寿命
			float speed = rnd.NextFloatRange(speed_Min, speed_Max);
			particle->velocity_ = particle->direction_ * speed;

			float size = rnd.NextFloatRange(size_Min, size_Max);
			particle->size_Origin_ = { size, size };
			particle->size_ = particle->size_Origin_;

			// 座標・色
			particle->position_ = emitter_ + particle->direction_ * size;
			particle->color_ = color;

			particle->time_ = rnd.NextUIntRange(deathtime_Min, deathtime_Max);
			particle->count_ = 0;

			particle->isAlive_ = true;

			count++;
		}
	}
}

void Pop::Update() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			particle->count_++;
			if (particle->count_ >= particle->time_) {
				particle->isAlive_ = false;
			}
			else {
				float t = std::clamp(
					static_cast<float>(particle->count_) /
					static_cast<float>(particle->time_),
					0.0f, 1.0f);
				// 移動
				Vector2 currentVelocity = Vector2(Math::Lerp(t, particle->velocity_.x, 0.0f), Math::Lerp(t, particle->velocity_.y, 0.0f));
				particle->position_ += currentVelocity;
				//// 色
				//particle->color_ = Vector4(
				//    1.0f, 1.0f, 1.0f,
				//	Math::Lerp(t,1.0f, 0.0f));

				// サイズ
				float size = Math::Lerp(t, particle->size_Origin_.x, 0.0f);
				particle->size_ = { size, size };
			}
		}
	}
}

void Pop::Draw() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			//float top = particle->position_.y + particle->size_.y * 0.5f;
			//float bottom = particle->position_.y - particle->size_.y * 0.5f;
			//float left = particle->position_.x - particle->size_.x * 0.5f;
			//float right = particle->position_.x + particle->size_.x * 0.5f;
			//
			//Vector2 bottomLeft = { left, bottom };
			//Vector2 topLeft = { left, top };
			//Vector2 bottomRight = { right, bottom };
			//Vector2 topRight = { right, top };
			//
			//TOMATOsEngine::DrawLine3D(bottomLeft, topLeft, Color(particle->color_));
			//TOMATOsEngine::DrawLine3D(topLeft, topRight, Color(particle->color_));
			//TOMATOsEngine::DrawLine3D(topRight, bottomRight, Color(particle->color_));
			//TOMATOsEngine::DrawLine3D(bottomRight, bottomLeft, Color(particle->color_));

			Vector2 center = particle->position_;

			Vector2 start = center - particle->direction_ * particle->size_.y;
			Vector2 end = center + particle->direction_ * particle->size_.y;

			TOMATOsEngine::DrawLine3D(start, end, Color(particle->color_));
		}
	}
}
