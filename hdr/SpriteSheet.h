#pragma once

#include <map>

#include "SFML/Graphics.hpp"
#include "pugixml/pugixml.hpp"

#define SPRITESDATA SpriteSheetData::Instance().SpritesData
#define TEXTURE SpriteSheetData::Instance().GeneralSpriteSheet

class SpriteSheetData
{
public:
	static SpriteSheetData& Instance() {
		static SpriteSheetData theSSD;
		return theSSD;
	}

	sf::Texture GeneralSpriteSheet;

	struct SpriteData {
		sf::Vector2i position;
		sf::Vector2i size;
		bool rotated;
	};
	std::map<std::string, SpriteData> SpritesData;

private:
	SpriteSheetData() { ReadFileAndPopulate(); }
	SpriteSheetData(SpriteSheetData const&);
	SpriteSheetData& operator= (SpriteSheetData const&) {}
	~SpriteSheetData() {}

	void ReadFileAndPopulate()
	{
		// SpriteSheet with general stuff:
		GeneralSpriteSheet.loadFromFile("images/shapes.png");
		//GeneralSpriteSheet.setSmooth(true); // las luces necesitan que sea Smooth

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file("images/shapes.xml");
		pugi::xml_node sprites = doc.child("TextureAtlas");

		std::string auxName;
		SpriteData auxData;

		if (result) {
			for (pugi::xml_node s = sprites.child("sprite"); s; s = s.next_sibling("sprite"))
			{
				auxName = s.attribute("n").value();
				auxData.position.x = s.attribute("x").as_int();
				auxData.position.y = s.attribute("y").as_int();
				auxData.size.x = s.attribute("w").as_int();
				auxData.size.y = s.attribute("h").as_int();
				s.attribute("r") ? auxData.rotated = true : auxData.rotated = false;

				SpritesData.insert(std::pair<std::string, SpriteData>(auxName, auxData));
			}
		}
	}
};