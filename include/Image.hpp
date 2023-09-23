#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <cstdint>
#include <exception>
#include <string>

class Image {
   private:
    int32_t _width;
    int32_t _height;
    int32_t _channels;
    unsigned char* _pixels;

   public:
    Image(void);
    Image(const std::string& filename);
    Image(const Image& other);
    ~Image();

    Image& operator=(const Image& other);

    int32_t getWidth(void);
    int32_t getHeight(void);
    int32_t getChannels(void);
    int32_t getPixelColor(int32_t posX, int32_t posY);

    void load(const std::string& filename);

    class FailedToLoadImage : public std::exception {
       public:
        const char* what() const throw();
    };
};

#endif /* IMAGE_HPP */
