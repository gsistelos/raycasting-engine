#include "Player.hpp"

#include <cmath>
#include <cstring>

Player::Player(double& deltaTime, Map& map)
    : posX(0), posY(0), dirX(0), dirY(0), planeX(0), planeY(0), moveSpeed(5.0f), rotSpeed(3.0f), cursorSpeed(0.002f), walkDir(0), sideDir(0), rotDir(0), cursorDir(0), deltaTime(deltaTime), map(map) {
}

Player::~Player() {
}

void Player::load(void) {
    // Find player position inside map

    for (size_t y = 0; y < map.grid.size(); y++) {
        for (size_t x = 0; x < map.grid[y].size(); x++) {
            if (!strchr("NSWE", map.grid[y][x]))
                continue;

            if (posX != 0 && posY != 0)
                throw Player::MoreThanOnePlayerFound();
            posX = x + 0.5;
            posY = y + 0.5;
        }
    }

    if (posX == 0 && posY == 0)
        throw Player::NoPlayerFound();

    switch (map.grid[(size_t)posY][(size_t)posX]) {
        case 'N':
            dirX = 0;
            dirY = -1;
            planeX = -0.66;
            planeY = 0;
            break;
        case 'S':
            dirX = 0;
            dirY = 1;
            planeX = 0.66;
            planeY = 0;
            break;
        case 'W':
            dirX = -1;
            dirY = 0;
            planeX = 0;
            planeY = 0.66;
            break;
        default:
            dirX = 1;
            dirY = 0;
            planeX = 0;
            planeY = -0.66;
    }

    map.grid[(size_t)posY][(size_t)posX] = '0';
}

void Player::update(void) {
    float moveStep = deltaTime * moveSpeed * walkDir;
    float sideStep = deltaTime * moveSpeed * sideDir;

    float nextX = posX + (dirX * moveStep) + (dirY * sideStep);
    float nextY = posY + (dirY * moveStep) - (dirX * sideStep);

    if (map.grid[(size_t)posY][(size_t)nextX] == '0')
        posX = nextX;
    if (map.grid[(size_t)nextY][(size_t)posX] == '0')
        posY = nextY;

    float rotStep = deltaTime * (rotSpeed * rotDir) + (cursorSpeed * cursorDir);

    float oldDirX = dirX;
    dirX = dirX * cos(rotStep) - dirY * sin(rotStep);
    dirY = oldDirX * sin(rotStep) + dirY * cos(rotStep);

    float oldPlaneX = planeX;
    planeX = planeX * cos(rotStep) - planeY * sin(rotStep);
    planeY = oldPlaneX * sin(rotStep) + planeY * cos(rotStep);
}

const char* Player::NoPlayerFound::what() const throw() {
    return "No player found";
}

const char* Player::MoreThanOnePlayerFound::what() const throw() {
    return "More than one player found";
}
