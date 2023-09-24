#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image::Image(void) : _width(0), _height(0), _channels(0), _pixels(nullptr) {
}

Image::Image(const std::string& filename) : _pixels(nullptr) {
    load(filename);
}

Image::Image(const Image& other) : _pixels(nullptr) {
    *this = other;
}

Image::~Image() {
    stbi_image_free(_pixels);
}

Image& Image::operator=(const Image& other) {
    if (this == &other)
        return *this;

    _width = other._width;
    _height = other._height;
    _channels = other._channels;

    stbi_image_free(_pixels);

    if (other._pixels == nullptr) {
        _pixels = nullptr;
    } else {
        size_t data_size = (_height * _width + _height) * 4;

        _pixels = new unsigned char[data_size];
        memcpy(_pixels, other._pixels, data_size);
    }

    return *this;
}

int32_t Image::getWidth(void) const {
    return _width;
}

int32_t Image::getHeight(void) const {
    return _height;
}

int32_t Image::getChannels(void) const {
    return _channels;
}

int32_t Image::getPixelColor(int32_t pos_x, int32_t pos_y) const {
    return *(int32_t*)(_pixels + (_height * pos_y + pos_x) * 4);
}

void Image::load(const std::string& filename) {
    stbi_image_free(_pixels);

    _pixels = stbi_load(filename.c_str(), &_width, &_height, &_channels, 4);
    if (_pixels == nullptr)
        throw Image::FailedToLoadImage();
}

const char* Image::FailedToLoadImage::what() const throw() {
    return "Failed to load image";
}
