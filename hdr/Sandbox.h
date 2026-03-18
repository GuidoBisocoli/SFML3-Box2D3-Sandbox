#pragma once

#include <vector>

#include "SFML/Graphics.hpp"
#include "box2d/box2d.h"

const unsigned int SCALE = 64.f; // 1m = 64px;
const unsigned int INITIALWIDTH = 1280;
const unsigned int INITIALHEIGHT = 800;

class Shape;

class Sandbox
{
public:
	Sandbox();
	~Sandbox();

	bool loop();

	static b2WorldId getWorldId() { return worldId; }
private:
	// SFML
	unsigned int screenWidth = INITIALWIDTH;
	unsigned int screenHeight = INITIALHEIGHT;

	sf::RenderWindow window;
	sf::View view;
	sf::Clock deltaClock;
	sf::Time deltaTime;
	sf::Font font;
	sf::Text text;

	// Background
	sf::Texture* bgTexture;
	sf::Sprite* bgSprite;

	// Box2D
	static b2WorldId worldId;

	// Sandbox
	b2BodyId groundBodyId = b2_nullBodyId;
	b2BodyId rightWallBodyId = b2_nullBodyId;
	void adjustLimitsForNewResolution();
	bool fullscreen = false;
	void toggleFullScreen();
	void createSandbox();

	// Mouse grab
	b2BodyId mouseBody = b2_nullBodyId;
	b2JointId mouseJointId = b2_nullJointId;
	void getBodyAtMousePos();
	void releaseBodyAtMousePos();
	void moveBodyAtMousePos(b2Vec2 p);

	// Other
	void scaleBackgroundToFitScreen();
	b2Vec2 getMousePos();
	void resizeScreen(unsigned int x, unsigned int y);
};

