#pragma once

#include <cstdint>
#include <memory>
#include <array>

#include "Math/MathUtils.h"
#include "TextureHandle.h"

class Number {
private:
	struct Particle {
		int number_;
		Vector2 position_;
		Vector4 color_;
		Vector2 velocity_;
		float size_;
		float size_Origin_;
		float spacing_;
		float spacing_Origin_;
		uint32_t time_;
		uint32_t count_;
		bool isAlive_;
	};
public:
	void Initialize();
	void Create(const Vector2 emitter, Vector4 color, int number);
	void Update();
	void Draw();

private:
	Vector2 emitter_;
	std::array<std::unique_ptr<Particle>, 200> particles_;
};
