#include "ShapeCreator.h"

#include "Sandbox.h"
#include "FakeRandom.h"
#include "Useful.h"

void ShapeCreator::createShape(b2Vec2 position, ShapeType st)
{
	sf::Sprite s(TEXTURE);

	std::string filename = getRandomShape(st);

	std::map<std::string, SpriteSheetData::SpriteData>::const_iterator results = SpriteSheetData::Instance().SpritesData.find(filename);
	sf::Rect<int> rect(results->second.position, results->second.size);

	// SFML
	s.setTextureRect(rect);
	sf::FloatRect spriteSize = s.getLocalBounds();
	s.setOrigin({ spriteSize.size.x / 2, spriteSize.size.y / 2 });

	// Box2D
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = position;
	bodyDef.fixedRotation = false;

	b2BodyId bodyId = b2CreateBody(Sandbox::getWorldId(), &bodyDef);
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	b2Polygon b2Shape = b2MakeBox(sfFloatToBoxFloat(spriteSize.size.x) / 2.f, sfFloatToBoxFloat(spriteSize.size.y) / 2.f); // toma half-widths
	b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &b2Shape);

	shapes.push_back(new Shape{ s, bodyId });
}

std::string ShapeCreator::getRandomShape(ShapeType st)
{
	// randomness
	bool createCrate = getRandomBool();
	int number = getRandom0_9();
	bool big = getRandomBool();

	if (st == ShapeType::any) goto any;
	else if (st == ShapeType::crate) goto crate;
	else if (st == ShapeType::box) goto box;
	else goto error;

any:
	if (createCrate) goto crate;
	else goto box;

crate:
	switch (number)
	{
	case 0: return "crate_0.png";
	case 1: return "crate_1.png";
	case 2: return "crate_2.png";
	case 3: return "crate_3.png";
	case 4: return "crate_4.png";
	case 5: return "crate_5.png";
	case 6: return "crate_6.png";
	case 7: return "crate_7.png";
	case 8: return "crate_8.png";
	case 9: return "crate_9.png";
	}

	goto error;
box:
	if (big) return "box_big.png";
	else return "box_small.png";

error:
	return "Error in Sandbox::getRandomShape(...)";
}

void ShapeCreator::updateShapes()
{
	for (Shape* s : shapes) {
		float posX = SCALE * b2Body_GetPosition(s->bodyId).x;
		float posY = SCALE * b2Body_GetPosition(s->bodyId).y;
		s->sprite.setPosition(sf::Vector2f(posX, posY));

		b2Rot rot = b2Body_GetRotation(s->bodyId);
		sf::Angle a = sf::radians(b2Rot_GetAngle(rot));
		s->sprite.setRotation(a);
	}
}

void ShapeCreator::drawShapes(sf::RenderWindow* window)
{
	for (Shape* s : shapes)
		window->draw(s->sprite);
}

void ShapeCreator::repositionShapes(sf::Vector2u previousSize, sf::Vector2u newSize)
{
	float proportion_x = (float)newSize.x / (float)previousSize.x;
	float proportion_y = (float)newSize.y / (float)previousSize.y;

	float x;
	float y;
	for (Shape* s : shapes) {
		x = b2Body_GetPosition(s->bodyId).x * proportion_x;
		y = b2Body_GetPosition(s->bodyId).y * proportion_y;
		b2Body_SetTransform(s->bodyId, b2Vec2{ x, y }, b2Body_GetRotation(s->bodyId));
		b2Body_SetAwake(s->bodyId, true);
	}
}
