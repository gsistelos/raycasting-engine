#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <cstdint>

#include "Map.hpp"

class Player {
   public:
    float posX, posY;
    float dirX, dirY;
    float planeX, planeY;
    float moveSpeed;
    float rotSpeed;
    float cursorSpeed;

    int32_t walkDir;
    int32_t sideDir;
    float rotDir;
    float cursorDir;

    double& deltaTime;
    Map& map;

    Player(double& deltaTime, Map& map);
    ~Player();

    void load(void);
    void update(void);

    class NoPlayerFound : public std::exception {
       public:
        const char* what() const throw();
    };

    class MoreThanOnePlayerFound : public std::exception {
       public:
        const char* what() const throw();
    };
};

#endif /* PLAYER_HPP */
