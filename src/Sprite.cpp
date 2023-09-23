#include "Sprite.hpp"

Sprite::Sprite(void) : posX(0), posY(0) {
}

Sprite::Sprite(float posX, float posY, std::string const& filename) : posX(posX), posY(posY), texture(filename) {
}

Sprite::Sprite(const Sprite& other) : posX(other.posX), posY(other.posY), texture(other.texture) {
}

Sprite::~Sprite() {
}

Sprite& Sprite::operator=(const Sprite& other) {
    if (this == &other)
        return *this;

    posX = other.posX;
    posY = other.posY;
    texture = other.texture;

    return *this;
}

void Sprite::load(float posX, float posY, const std::string& filename) {
    this->posX = posX;
    this->posY = posY;

    texture.load(filename);
}
