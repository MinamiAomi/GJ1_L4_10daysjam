#include "Particle/Splash.h"

#include <iomanip>
#include <sstream>

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"

void Splash::Initialize() {
	emitter_ = { 0.0f, 0.0f };
	for (auto& particle : particles_) {
		particle = std::make_unique<Particle>();
	}
}

void Splash::Create(const Vector2& emitter, const Vector2& direction, Vector4 color, uint32_t MaxParticle) {
	Random::RandomNumberGenerator rnd{};
	emitter_ = emitter;
	const uint32_t deathtime_Min = 8;
	const uint32_t deathtime_Max = 16;
	const float speed_Min = 0.2f;
	const float speed_Max = 1.3f;
	const float size_Min = 0.1f;
	const float size_Max = 0.5f;
	const float spreadRadians = 40.0f * Math::ToRadian;
	const uint32_t count_Max = MaxParticle;
	uint32_t count = 0;

	for (auto& particle : particles_) {
		if (count < count_Max && !particle->isAlive_) {
			// 座標
			particle->position_ = emitter_;
			// 色
			particle->color_ = color;

			float randomAngle = rnd.NextFloatRange(-spreadRadians, spreadRadians);

			float cos_a = std::cosf(randomAngle);
			float sin_a = std::sinf(randomAngle);

			Vector2 finalDirection;
			finalDirection.x = direction.x * cos_a - direction.y * sin_a;
			finalDirection.y = direction.x * sin_a + direction.y * cos_a;

			float speed = rnd.NextFloatRange(speed_Min, speed_Max);
			particle->velocity_ = finalDirection * speed;
			// 加速度
			particle->acceleration_ = { 0.0f, 0.0f };
			// サイズ
			float size = rnd.NextFloatRange(size_Min, size_Max);
			particle->size_Origin_ = { size, size };
			particle->size_ = particle->size_Origin_;
			// 寿命
			particle->time_ = rnd.NextUIntRange(deathtime_Min, deathtime_Max);
			particle->count_ = 0;

			particle->isAlive_ = true;

			count++;
		}
	}
}

void Splash::Update() {
	const float kGravity = -0.2f;
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
				particle->acceleration_.y = kGravity;
				particle->velocity_ += particle->acceleration_;
				particle->position_ += particle->velocity_;
				// 色
				particle->color_ = Vector4(
					1.0f, 1.0f, 1.0f,
					Math::Lerp(t, 1.0f, 0.0f));

				// サイズ
				float size = Math::Lerp(t,  particle->size_Origin_.x, 0.0f);
				particle->size_ = { size, size };
			}
		}
	}
}

void Splash::Draw() {
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			float top = particle->position_.y + particle->size_.y * 0.5f;
			float bottom = particle->position_.y - particle->size_.y * 0.5f;
			float left = particle->position_.x - particle->size_.x * 0.5f;
			float right = particle->position_.x + particle->size_.x * 0.5f;

			Vector2 bottomLeft = { left, bottom };
			Vector2 topLeft = { left, top };
			Vector2 bottomRight = { right, bottom };
			Vector2 topRight = { right, top };

			TOMATOsEngine::DrawLine3D(bottomLeft, topLeft, Color(particle->color_));
			TOMATOsEngine::DrawLine3D(topLeft, topRight, Color(particle->color_));
			TOMATOsEngine::DrawLine3D(topRight, bottomRight, Color(particle->color_));
			TOMATOsEngine::DrawLine3D(bottomRight, bottomLeft, Color(particle->color_));
		}
	}
}
