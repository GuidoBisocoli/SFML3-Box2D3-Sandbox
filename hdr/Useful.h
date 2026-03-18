#pragma once

#include "Sandbox.h"

// static so I can add include them in any file without conflicts
static float sfFloatToBoxFloat(const float f) { return f / SCALE; }
static b2Vec2 sfVecToBoxVec(const sf::Vector2i& vec) {
	return b2Vec2{ (float)vec.x / SCALE, (float)vec.y / SCALE };
}
static b2Vec2 sfVecToBoxVec(const sf::Vector2f& vec) {
	return b2Vec2{ vec.x / SCALE, vec.y / SCALE };
}