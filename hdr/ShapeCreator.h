#pragma once

#include "SpriteSheet.h"

#include "SFML/Graphics.hpp"
#include "box2d/box2d.h"

#define SHAPECREATOR ShapeCreator::Instance()

class ShapeCreator
{
public:
	enum ShapeType { any, box, crate };

	static ShapeCreator& Instance() {
		static ShapeCreator theSC;
		return theSC;
	}

	void createShape(b2Vec2 position, ShapeType st);
	void updateShapes();
	void drawShapes(sf::RenderWindow* window);
	void repositionShapes(sf::Vector2u previousSize, sf::Vector2u newSize);
private:	
	ShapeCreator() { }
	ShapeCreator(ShapeCreator const&);
	ShapeCreator& operator= (ShapeCreator const&) {}
	~ShapeCreator() {}

	struct Shape {
		sf::Sprite sprite;
		b2BodyId bodyId;
	};
	std::vector<Shape*> shapes;

	std::string getRandomShape(ShapeType st);
};