#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <cstdint>

#include "Map.hpp"

class Player {
  public:
    float pos_x, pos_y;
    float dir_x, dir_y;
    float plane_x, plane_y;

    float move_speed;
    float rot_speed;
    float cursor_speed;

    int32_t walk_dir;
    int32_t side_dir;

    float rot_dir;
    float cursor_dir;

    Player(Map& map);
    ~Player();

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
