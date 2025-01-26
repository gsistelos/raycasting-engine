#ifndef IMAGE_HPP
#define IMAGE_HPP

class Image {
  public:
    Image(void);
    Image(const char* filename);
    Image(const Image& other);
    virtual ~Image();

    Image& operator=(const Image& other);

    int getWidth(void) const;
    int getHeight(void) const;
    int getPixelColor(int pos_x, int pos_y) const;

    void load(const char* filename);

  private:
    int _width;
    int _height;
    unsigned char* _pixels;

    static const int _channels = 4;
};

inline int Image::getWidth(void) const { return _width; }

inline int Image::getHeight(void) const { return _height; }

inline int Image::getPixelColor(int pos_x, int pos_y) const {
    return *(int*)(_pixels + (_height * pos_y + pos_x) * _channels);
}

#endif /* IMAGE_HPP */
