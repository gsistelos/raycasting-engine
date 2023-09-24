#include "Player.hpp"

#include <cmath>
#include <cstring>

Player::Player(Map& map)
    : pos_x(0), pos_y(0), move_speed(5.0f), rot_speed(3.0f), cursor_speed(0.002f), walk_dir(0), side_dir(0), rot_dir(0), cursor_dir(0) {
    // Find player position inside map

    for (size_t y = 0; y < map.grid.size(); y++) {
        for (size_t x = 0; x < map.grid[y].size(); x++) {
            if (!strchr("NSWE", map.grid[y][x]))
                continue;

            if (pos_x != 0 && pos_y != 0)
                throw Player::MoreThanOnePlayerFound();
            pos_x = x + 0.5;
            pos_y = y + 0.5;
        }
    }

    if (pos_x == 0 && pos_y == 0)
        throw Player::NoPlayerFound();

    switch (map.grid[(size_t)pos_y][(size_t)pos_x]) {
        case 'N':
            dir_x = 0;
            dir_y = -1;
            plane_x = -0.66;
            plane_y = 0;
            break;
        case 'S':
            dir_x = 0;
            dir_y = 1;
            plane_x = 0.66;
            plane_y = 0;
            break;
        case 'W':
            dir_x = -1;
            dir_y = 0;
            plane_x = 0;
            plane_y = 0.66;
            break;
        default:
            dir_x = 1;
            dir_y = 0;
            plane_x = 0;
            plane_y = -0.66;
    }

    map.grid[(size_t)pos_y][(size_t)pos_x] = '0';
}

Player::~Player() {
}

const char* Player::NoPlayerFound::what() const throw() {
    return "No player found";
}

const char* Player::MoreThanOnePlayerFound::what() const throw() {
    return "More than one player found";
}
