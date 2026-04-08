#include"TextureManager.h"
#include<iostream>

bool TextureManager::loadTexture(const std::string& name, const std::string& fileName) {
	if (!texture.loadFromFile(fileName)) {

		return false;
	}

	textures_map[name] = texture;

	textures_map[name].setSmooth(true);
	textures_map[name].generateMipmap();
	return true;
}

bool TextureManager::loadFont(const std::string& name, const std::string& fileName) {
	
	auto newFont = std::make_unique<sf::Font>();

	if (!newFont->loadFromFile(fileName)) {

		return false;
	}
	font_map[name] = std::move(newFont);
	return true;

}

sf::Texture& TextureManager::getTexture(const std::string& name) {

	auto it = textures_map.find(name);
	if (it != textures_map.end()) {
		return it->second;
	}
	throw std::runtime_error("Texture not found: " + name);
}

sf::Font& TextureManager::getFont(const std::string& name) {
	
	auto it = font_map.find(name);

	if (it != font_map.end()) {

		return *(it->second); // Trả về tham chiếu đến Font nằm trong unique_ptr
	}

	throw std::runtime_error("Font not found: " + name);
}

void TextureManager::defineFrame(const std::string& name, int left, int top) {
	frames_map[name] = sf::IntRect(left, top, FRAME_SIZE, FRAME_SIZE);
}

void TextureManager::setupChessAtlas() {
	std::string pieceNames[6] = { "Pawn","Rook","Knight","Bishop","Queen","King" };
	for (int i = 0; i < 6; i++) {
		defineFrame(pieceNames[i] + "_WHITE", i * FRAME_SIZE, 0);
		defineFrame(pieceNames[i] + "_BLACK", i * FRAME_SIZE, FRAME_SIZE);
	}
}

sf::Sprite TextureManager::createSprite(const std::string& textureName, const std::string& frameName) {
	sf::Sprite sprite;
	sprite.setTexture(getTexture(textureName));
	auto it = frames_map.find(frameName);
	if (it != frames_map.end()) {
		sprite.setTextureRect(it->second);
	}
	else {
		throw std::runtime_error("Frame not Found");
	}
	return sprite;
}
