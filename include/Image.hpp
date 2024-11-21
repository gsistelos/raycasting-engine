#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <cstdint>
#include <exception>
#include <string>

class Image {
  public:
    Image(void);
    Image(const std::string& filename);
    Image(const Image& other);
    virtual ~Image();

    Image& operator=(const Image& other);

    int32_t getWidth(void) const;
    int32_t getHeight(void) const;
    int32_t getChannels(void) const;
    int32_t getPixelColor(int32_t pos_x, int32_t pos_y) const;

    void load(const std::string& filename);

    class FailedToLoadImage : public std::exception {
      public:
        const char* what() const throw();
    };

  private:
    int32_t _width;
    int32_t _height;
    int32_t _channels;
    unsigned char* _pixels;
};

#endif /* IMAGE_HPP */
