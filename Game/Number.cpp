#include "Number.h"

#include "Math/Color.h"
#include "Engine/TOMATOsEngine.h"
#include "Math/Random.h"
#include "HexagonSevenSegmentDisplay.h"

void Number::Initialize()
{
	emitter_ = { 0.0f, 0.0f };
	for (auto& particle : particles_) {
		particle = std::make_unique<Particle>();
	}
}

void Number::Create(const Vector2 emitter, Vector4 color, int number)
{
	Random::RandomNumberGenerator rnd{};
	const uint32_t deathtime = 20;
	const float size = 0.1f;
	const float speed = 0.05f;
	const float spacing = 2.0f;

	emitter_ = emitter;

	for (auto& particle : particles_) {

		if (!particle->isAlive_) {
			particle->number_ = number;

			particle->velocity_ = Vector2(0.0f, 1.0f) * speed;

			particle->size_Origin_ = size;
			particle->size_ = particle->size_Origin_;

			// 座標・色
			particle->position_ = emitter_;
			particle->color_ = color;
			
			particle->spacing_Origin_ = spacing;
			particle->spacing_ = spacing;

			particle->time_ = deathtime;
			particle->count_ = 0;

			particle->isAlive_ = true;
			break;
		}
	}
}

void Number::Update()
{
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
				Vector2 currentVelocity = Vector2(0.0f, Math::Lerp(t, particle->velocity_.y, 0.0f));
				particle->position_ += currentVelocity;
				// サイズ
				float size = Math::Lerp(t, particle->size_Origin_, 0.0f);
				particle->size_ = size;

				float spacing = Math::Lerp(t, particle->spacing_Origin_, 0.0f);
				particle->spacing_= spacing;
			}
		}
	}
}

void Number::Draw()
{
	for (auto& particle : particles_) {
		if (particle->isAlive_) {
			HexagonSevenSegmentDisplay::GetInstance()->DrawWirefradme(
				particle->number_,
				particle->position_,
				particle->size_,
				particle->spacing_,
				Color(particle->color_)
			);
		}
	}
}
