#ifndef GAME_HPP
#define GAME_HPP

#include <GLFW/glfw3.h>

#include <map>
#include <string>

#include "Image.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Sprite.hpp"

class Game {
   public:
    GLFWwindow* window;
    int32_t window_width, window_height;

    double time;
    double delta_time;

    Map* map;
    Player* player;

    std::map<char, Image> textures;
    std::vector<Sprite> sprites;
    std::vector<float> dist_buffer;
    std::vector<int32_t> sprite_order;
    std::vector<float> sprite_distance;

    Game(void);
    ~Game();

    void init(const std::string& filename);
    void loadTexturesAndSprites(const std::string& filename);

    void renderWalls(void);
    void renderFloorAndCeiling(void);
    void renderSprites(void);
    void render(void);

    void loop(void);

    void (Game::*gameState)(void);

    void paused(void);
    void update(void);

    class CouldntOpenMap : public std::exception {
       public:
        const char* what() const throw();
    };

    class InvalidContent : public std::exception {
       public:
        const char* what() const throw();
    };

    class FailedToInitGame : public std::exception {
       public:
        const char* what() const throw();
    };
};

#endif /* GAME_HPP */
