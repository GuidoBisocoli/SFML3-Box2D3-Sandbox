#include "Sandbox.h"

#include <optional>
#include <iostream>
#include <string>

#include "SFML/Graphics.hpp"

#include "SpriteSheet.h"
#include "ShapeCreator.h"
#include "Useful.h"

b2WorldId Sandbox::worldId = {};

//-----------//
/// SANDBOX ///
//-----------//
Sandbox::Sandbox() : text(font, "", 24)
{
	// window & view
	window.create(sf::VideoMode({ screenWidth, screenHeight }), "Sandbox", sf::Style::Close, sf::State::Windowed);
	view.setSize({ (float)screenWidth, (float)screenHeight });

	// world
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = { 0.f, 9.8f };
	worldId = b2CreateWorld(&worldDef);

	// background
	bgTexture = new sf::Texture();
	bgTexture->loadFromFile("images/Background.png");
	bgSprite = new sf::Sprite(*bgTexture);
	scaleBackgroundToFitScreen();

	// Font
	font.openFromFile("arial.ttf");
	text.setString("C: spawn random crate\nB: spawn random box\nSpace: spawn random crate or box\nF: toggle fullscreen\nGrab them with the mouse");
	text.setPosition(sf::Vector2f{ 1.f, 1.f });

	// ShapeCreator Initialization
	SHAPECREATOR;

	// Sandbox
	createSandbox();
}

Sandbox::~Sandbox()
{
	delete bgSprite;
	delete bgTexture;

	if (B2_IS_NON_NULL(mouseJointId)) {
		b2DestroyJoint(mouseJointId);
		mouseJointId = b2_nullJointId;
	}

	if (B2_IS_NON_NULL(mouseJointId)) {
		b2DestroyBody(mouseBody);
		mouseBody = b2_nullBodyId;
	}

	b2DestroyWorld(worldId);
	worldId = b2_nullWorldId;
}

bool Sandbox::loop()
{
	static bool running = true;
	deltaTime = deltaClock.restart();

	// input
	while (const std::optional event = window.pollEvent())
	{
		// event
		if (event->is<sf::Event::Closed>())
			running = false;

		// keyboard
		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
			if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
				running = false;
			if (keyPressed->scancode == sf::Keyboard::Scancode::Space)
				SHAPECREATOR.createShape(getMousePos(), SHAPECREATOR.ShapeType::any);
			if (keyPressed->scancode == sf::Keyboard::Scancode::C)
				SHAPECREATOR.createShape(getMousePos(), SHAPECREATOR.ShapeType::crate);
			if (keyPressed->scancode == sf::Keyboard::Scancode::B)
				SHAPECREATOR.createShape(getMousePos(), SHAPECREATOR.ShapeType::box);
			if (keyPressed->scancode == sf::Keyboard::Scancode::F)
				toggleFullScreen();
		}

		// mouse
		if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
			if (mousePressed->button == sf::Mouse::Button::Left)
				getBodyAtMousePos();
		}
		else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
			if (mouseReleased->button == sf::Mouse::Button::Left)
				releaseBodyAtMousePos();
		}
		else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
				moveBodyAtMousePos(sfVecToBoxVec(mouseMoved->position));
		}
	}

	// loop
	if (running) {
		// shapes
		SHAPECREATOR.updateShapes();

		// Box2D
		b2World_Step(worldId, deltaTime.asSeconds(), 4);

		// draw
		window.clear();
		window.draw(*bgSprite);
		SHAPECREATOR.drawShapes(&window);
		window.draw(text);
		window.display();
	}

	else window.close();

	return running;
}

void Sandbox::adjustLimitsForNewResolution()
{
	b2Vec2 newPos = b2Vec2{ sfFloatToBoxFloat(screenWidth / 2) , sfFloatToBoxFloat(screenHeight) + 1.f };
	b2Body_SetTransform(groundBodyId, newPos, b2Body_GetRotation(groundBodyId));

	newPos = b2Vec2{ sfFloatToBoxFloat(screenWidth) + 1.f , sfFloatToBoxFloat(screenHeight / 2) };
	b2Body_SetTransform(rightWallBodyId, newPos, b2Body_GetRotation(rightWallBodyId));
}

void Sandbox::toggleFullScreen()
{
	if (fullscreen) {
		resizeScreen(INITIALWIDTH, INITIALHEIGHT);

		fullscreen = false;
	}
	else {
		sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

		resizeScreen(desktopMode.size.x, desktopMode.size.y);

		fullscreen = true;
	}
}

void Sandbox::createSandbox()
{
	// floor
	b2Vec2 auxPos = b2Vec2{ sfFloatToBoxFloat(screenWidth / 2) , sfFloatToBoxFloat(screenHeight) + 1.f };
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_staticBody;
	bodyDef.position = auxPos;
	groundBodyId = b2CreateBody(worldId, &bodyDef);

	b2Polygon groundBox = b2MakeBox(2000.f, 1.0f); // it takes half-widths
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	b2CreatePolygonShape(groundBodyId, &shapeDef, &groundBox);

	// ceiling
	auxPos = b2Vec2{ sfFloatToBoxFloat(screenWidth / 2) , -1.f };
	bodyDef.position = auxPos;
	b2BodyId ceilingBodyId = b2CreateBody(worldId, &bodyDef);
	b2Polygon ceilingBox = b2MakeBox(2000.f, 1.0f);
	b2CreatePolygonShape(ceilingBodyId, &shapeDef, &ceilingBox);

	// some restitution for the walls
	shapeDef.material.restitution = 0.4f;

	// left wall
	auxPos.x = -1.f;
	auxPos.y = sfFloatToBoxFloat(screenHeight / 2);
	bodyDef.position = auxPos;
	b2BodyId leftWallBodyId = b2CreateBody(worldId, &bodyDef);

	b2Polygon leftWallBox = b2MakeBox(1.f, 2000.f);
	b2CreatePolygonShape(leftWallBodyId, &shapeDef, &leftWallBox);

	// right wall
	auxPos.x = sfFloatToBoxFloat(screenWidth) + 1.f;
	auxPos.y = sfFloatToBoxFloat(screenHeight / 2);
	bodyDef.position = auxPos;
	rightWallBodyId = b2CreateBody(worldId, &bodyDef);

	b2Polygon rightWallBox = b2MakeBox(1.f, 2000.f);
	b2CreatePolygonShape(rightWallBodyId, &shapeDef, &rightWallBox);
}

//---------//
/// QUERY ///
//---------//
struct QueryContext
{
	b2Vec2 point;
	b2BodyId bodyId = b2_nullBodyId;
};

bool QueryCallback(b2ShapeId shapeId, void* context)
{
	QueryContext* queryContext = static_cast<QueryContext*>(context);

	b2BodyId bodyId = b2Shape_GetBody(shapeId);
	b2BodyType bodyType = b2Body_GetType(bodyId);
	if (bodyType != b2_dynamicBody)
	{
		// continue query
		return true;
	}

	bool overlap = b2Shape_TestPoint(shapeId, queryContext->point);
	if (overlap)
	{
		// found shape
		queryContext->bodyId = bodyId;
		return false;
	}

	return true;
}

void Sandbox::getBodyAtMousePos()
{
	b2Vec2 mousePos = getMousePos();

	// Make a small box.
	b2AABB aabb;
	b2Vec2 d{ 0.001f, 0.001f };
	aabb.lowerBound = mousePos - d;
	aabb.upperBound = mousePos + d;

	// Query the world for overlapping shapes.
	QueryContext queryContext = { mousePos, b2_nullBodyId };
	b2World_OverlapAABB(worldId, aabb, b2DefaultQueryFilter(), QueryCallback, &queryContext);

	if (B2_IS_NON_NULL(queryContext.bodyId))
	{
		b2BodyDef bodyDef = b2DefaultBodyDef();
		mouseBody = b2CreateBody(worldId, &bodyDef);

		b2MouseJointDef mouseDef = b2DefaultMouseJointDef();
		mouseDef.bodyIdA = mouseBody;
		mouseDef.bodyIdB = queryContext.bodyId;
		mouseDef.target = mousePos;
		mouseDef.hertz = 10.0f;
		mouseDef.dampingRatio = 0.7f;
		mouseDef.maxForce = 1000.0f * b2Body_GetMass(queryContext.bodyId) * b2Length(b2World_GetGravity(worldId));
		mouseJointId = b2CreateMouseJoint(worldId, &mouseDef);

		b2Body_SetAwake(queryContext.bodyId, true);
	}
}

void Sandbox::releaseBodyAtMousePos()
{
	if (B2_IS_NON_NULL(mouseJointId))
	{
		b2DestroyJoint(mouseJointId);
		mouseJointId = b2_nullJointId;

		b2DestroyBody(mouseBody);
		mouseBody = b2_nullBodyId;
	}
}

void Sandbox::moveBodyAtMousePos(b2Vec2 p)
{
	if (B2_IS_NON_NULL(mouseJointId))
	{
		b2MouseJoint_SetTarget(mouseJointId, p);
		b2BodyId bodyIdB = b2Joint_GetBodyB(mouseJointId);
		b2Body_SetAwake(bodyIdB, true);
	}
}

//----------//
/// USEFUL ///
//----------//
void Sandbox::scaleBackgroundToFitScreen()
{
	sf::FloatRect spriteBounds = bgSprite->getLocalBounds();

	float scale_for_height = window.getSize().y / spriteBounds.size.y;

	// apply height scale to both axis to avoid distortion and fill the entire screen
	// (only if the image is wider the taller)
	bgSprite->setScale({ scale_for_height, scale_for_height });
}

b2Vec2 Sandbox::getMousePos()
{
	sf::Vector2i mousePosPxs = sf::Mouse::getPosition(window);
	sf::Vector2f mousePosCoords = window.mapPixelToCoords(mousePosPxs);

	return sfVecToBoxVec(mousePosCoords);
}

void Sandbox::resizeScreen(unsigned int x, unsigned int y)
{
	sf::Vector2u previous = window.getSize();

	window.close();
	if (fullscreen)
		window.create(sf::VideoMode({x, y}), "Sandbox", sf::Style::Close, sf::State::Windowed);
	else window.create(sf::VideoMode({ x, y }), "Sandbox", sf::Style::None, sf::State::Fullscreen);
	view.setSize({ (float)x, (float)y });

	screenWidth = (unsigned int)x;
	screenHeight = (unsigned int)y;

	adjustLimitsForNewResolution();
	scaleBackgroundToFitScreen();

	// move the floor and the right wall

	// move the shapes
	SHAPECREATOR.repositionShapes(previous, window.getSize());
}
