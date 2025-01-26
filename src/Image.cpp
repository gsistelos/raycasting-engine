#include "Image.hpp"

#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image::Image(void) : _width(0), _height(0), _pixels(nullptr) {}

Image::Image(const char* filename) : _pixels(nullptr) { load(filename); }

Image::Image(const Image& other) : _pixels(nullptr) { *this = other; }

Image::~Image() { stbi_image_free(_pixels); }

Image& Image::operator=(const Image& other) {
    if (this == &other)
        return *this;

    _width = other._width;
    _height = other._height;

    stbi_image_free(_pixels);

    if (other._pixels == nullptr) {
        _pixels = nullptr;
    } else {
        size_t data_size = _width * _height * _channels;

        _pixels = new unsigned char[data_size];
        memcpy(_pixels, other._pixels, data_size);
    }

    return *this;
}

void Image::load(const char* filename) {
    stbi_image_free(_pixels);

    _pixels = stbi_load(filename, &_width, &_height, nullptr, _channels);
    if (_pixels == nullptr)
        throw std::runtime_error("stbi_load failed");
}
