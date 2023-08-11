#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "Image.hpp"

class Sprite {
   public:
    float posX, posY;
    Image texture;

    Sprite(void);
    Sprite(float posX, float posY, const std::string& filename);
    Sprite(const Sprite& other);
    ~Sprite();

    Sprite& operator=(const Sprite& other);

    void load(float posX, float posY, const std::string& filename);
};

#endif /* SPRITE_HPP */
