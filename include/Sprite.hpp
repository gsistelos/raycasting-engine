#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "Image.hpp"

class Sprite : public Image {
  public:
    float pos_x, pos_y;

    Sprite(void);
    Sprite(const char* filename, float pos_x, float pos_y);
    Sprite(const Sprite& other);
    ~Sprite();

    Sprite& operator=(const Sprite& other);
};

#endif /* SPRITE_HPP */
