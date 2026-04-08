#pragma once
#include"SFML/Graphics.hpp"
#include"SFML/Audio.hpp"


const int FRAME_SIZE = 333;

class TextureManager {

private:
	sf::Texture	texture;
	sf::Font font;

	std::map<std::string, std::unique_ptr<sf::Font>> font_map;
	std::map<std::string, sf::Texture> textures_map;
	std::map<std::string, sf::IntRect> frames_map;

public:
	bool loadTexture(const std::string& name, const std::string& filename);
	bool loadFont(const std::string& name, const std::string& fileName);

	sf::Texture& getTexture(const std::string& name);
	sf::Font& getFont(const std::string& name);

	void defineFrame(const std::string& name, int left, int top);
	void setupChessAtlas();

	sf::Sprite createSprite(const std::string& textureName, const std::string& frameName);

};