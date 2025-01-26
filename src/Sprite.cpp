#include "Sprite.hpp"

Sprite::Sprite(void) : Image(), pos_x(0), pos_y(0) {}

Sprite::Sprite(const char* filename, float pos_x, float pos_y)
    : Image(filename), pos_x(pos_x), pos_y(pos_y) {}

Sprite::Sprite(const Sprite& other)
    : Image(other), pos_x(other.pos_x), pos_y(other.pos_y) {}

Sprite::~Sprite() {}

Sprite& Sprite::operator=(const Sprite& other) {
    if (this == &other)
        return *this;

    Image::operator=(other);

    pos_x = other.pos_x;
    pos_y = other.pos_y;

    return *this;
}
