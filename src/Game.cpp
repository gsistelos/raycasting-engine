#include "Game.hpp"

#include <GL/gl.h>
#include <algorithm>
#include <cmath>
#include <fstream>

Game::Game(void) : map(nullptr), player(nullptr) {}

Game::~Game() {
    delete map;
    delete player;
}

void Game::init(const std::string& filename) {
    loadTexturesAndSprites(filename);

    map = new Map(filename);
    player = new Player(*map);

    gameState = &Game::update;

    time = glfwGetTime();
}

void Game::loadTexturesAndSprites(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw Game::CouldntOpenMap();

    std::string line;
    while (std::getline(file, line, '\n')) {
        if (line.empty())
            continue;

        if (line == "MAP")
            break;

        if (line.length() < 3)
            throw Game::InvalidContent();

        if (!isspace(line[0]) && line[1] == ' ') {
            textures[line[0]] = Image(line.substr(2).c_str());
        } else {
            // Handle errors for each sprite coordinate

            int32_t pos[2];
            for (int32_t ipos = 0; ipos < 2; ipos++) {
                size_t end = line.find(' ');
                if (end > 3)
                    throw Game::InvalidContent();

                for (size_t i = 0; i < end; i++) {
                    if (!isdigit(line[i]))
                        throw Game::InvalidContent();
                }

                pos[ipos] = std::stof(line.substr(0, end));

                line.erase(0, end + 1);
            }

            sprites.push_back(Sprite(line.substr().c_str(), pos[0], pos[1]));
        }
    }

    file.close();

    // Vectors for sprite rendering calculations

    dist_buffer.resize(window.width);
    sprite_order.resize(sprites.size());
    sprite_distance.resize(sprites.size());
}

void Game::renderWalls(void) {
    for (int32_t x = 0; x < window.width; x++) {
        // Ray direction in current iteration

        float camera_x = 2 * x / float(window.width) - 1;

        float ray_dir_x = player->dir_x + player->plane_x * camera_x;
        float ray_dir_y = player->dir_y + player->plane_y * camera_x;

        // Reset map_x and map_y each iteration

        int32_t map_x = player->pos_x;
        int32_t map_y = player->pos_y;

        // Distance for each step

        float delta_dist_x = (ray_dir_x == 0) ? 1e30 : std::abs(1 / ray_dir_x);
        float delta_dist_y = (ray_dir_y == 0) ? 1e30 : std::abs(1 / ray_dir_y);

        // Closest interception distance and which direction to step

        int32_t step_x, step_y;
        float side_dist_x, side_dist_y;

        if (ray_dir_x < 0) {
            step_x = -1;
            side_dist_x = (player->pos_x - map_x) * delta_dist_x;
        } else {
            step_x = 1;
            side_dist_x = (map_x + 1.0f - player->pos_x) * delta_dist_x;
        }

        if (ray_dir_y < 0) {
            step_y = -1;
            side_dist_y = (player->pos_y - map_y) * delta_dist_y;
        } else {
            step_y = 1;
            side_dist_y = (map_y + 1.0f - player->pos_y) * delta_dist_y;
        }

        // Step until find a wall

        bool side;

        while (1) {
            if (side_dist_x < side_dist_y) {
                side_dist_x += delta_dist_x;
                map_x += step_x;
                side = 0;
            } else {
                side_dist_y += delta_dist_y;
                map_y += step_y;
                side = 1;
            }

            if (map->grid[map_y][map_x] != '0')
                break;
        }

        // Store the hit distance

        if (side)
            dist_buffer[x] = (side_dist_y - delta_dist_y);
        else
            dist_buffer[x] = (side_dist_x - delta_dist_x);

        // Information based on distance

        int32_t line_height = window.height / dist_buffer[x];

        int32_t draw_start = window.height / 2 - line_height / 2;
        if (draw_start < 0)
            draw_start = 0;

        int32_t draw_end = draw_start + line_height;
        if (draw_end > window.height)
            draw_end = window.height;

        // Position where the wall was hit

        float wall_x;

        if (side)
            wall_x = player->pos_x + dist_buffer[x] * ray_dir_x;
        else
            wall_x = player->pos_y + dist_buffer[x] * ray_dir_y;

        wall_x -= (int32_t)wall_x;

        // Wall texture

        Image* texture = &textures[map->grid[map_y][map_x]];

        // Convert the wall_x position into tex_x position

        int32_t tex_x = wall_x * texture->getWidth();
        if (!side && ray_dir_x > 0)
            tex_x = texture->getWidth() - tex_x - 1;
        if (side && ray_dir_y < 0)
            tex_x = texture->getWidth() - tex_x - 1;

        // Start and step for tex_y

        float step = 1.0f * texture->getHeight() / line_height;
        float tex_pos =
            (draw_start - window.height / 2 + line_height / 2) * step;

        // Render a vertical line

        for (int32_t y = draw_start; y < draw_end; y++) {
            int32_t tex_y = (int32_t)tex_pos & (texture->getHeight() - 1);
            tex_pos += step;

            int32_t color = texture->getPixelColor(tex_x, tex_y);
            if (side)
                color = (color >> 1) & 8355711;

            glColor3ubv((GLubyte*)&color);
            glVertex2i(x, y);
        }
    }
}

void Game::renderFloorAndCeiling(void) {
    // Floor and ceiling textures

    Image& floor_texture = textures['F'];
    Image& ceiling_texture = textures['C'];

    for (int32_t y = window.height / 2; y < window.height; y++) {
        // Ray direction for the floor

        float ray_dir_x0 = player->dir_x - player->plane_x;
        float ray_dir_y0 = player->dir_y - player->plane_y;
        float ray_dir_x1 = player->dir_x + player->plane_x;
        float ray_dir_y1 = player->dir_y + player->plane_y;

        // Horizontal distance from the camera to the floor for the current row

        float row_distance = 0.5f * window.height / (y - window.height / 2);

        // Starting position of the floor

        float floor_x = player->pos_x + row_distance * ray_dir_x0;
        float floor_y = player->pos_y + row_distance * ray_dir_y0;

        // How much to step from one pixel to the next

        float floor_step_x =
            row_distance * (ray_dir_x1 - ray_dir_x0) / window.width;
        float floor_step_y =
            row_distance * (ray_dir_y1 - ray_dir_y0) / window.width;

        for (int32_t x = 0; x < window.width; x++) {
            int32_t cell_x = (int32_t)(floor_x);
            int32_t cell_y = (int32_t)(floor_y);

            int32_t tex_x =
                (int32_t)(floor_texture.getWidth() * (floor_x - cell_x)) &
                (floor_texture.getWidth() - 1);
            int32_t tex_y =
                (int32_t)(floor_texture.getHeight() * (floor_y - cell_y)) &
                (floor_texture.getHeight() - 1);

            floor_x += floor_step_x;
            floor_y += floor_step_y;

            // Render floor and ceiling

            int32_t color = floor_texture.getPixelColor(tex_x, tex_y);
            color = (color >> 1) & 8355711;
            glColor3ubv((GLubyte*)&color);
            glVertex2i(x, y);

            color = ceiling_texture.getPixelColor(tex_x, tex_y);
            color = (color >> 1) & 8355711;
            glColor3ubv((GLubyte*)&color);
            glVertex2i(x, window.height - y - 1);
        }
    }
}

void Game::renderSprites(void) {
    // TODO: refactor sprite sorting

    for (size_t i = 0; i < sprites.size(); i++) {
        sprite_order[i] = i;
        sprite_distance[i] = ((player->pos_x - sprites[i].pos_x) *
                                  (player->pos_x - sprites[i].pos_x) +
                              (player->pos_y - sprites[i].pos_y) *
                                  (player->pos_y - sprites[i].pos_y));
    }

    std::vector<std::pair<double, int>> tmpSprites(sprites.size());
    for (size_t i = 0; i < sprites.size(); i++) {
        tmpSprites[i].first = sprite_distance[i];
        tmpSprites[i].second = sprite_order[i];
    }
    std::sort(tmpSprites.begin(), tmpSprites.end());
    for (size_t i = 0; i < sprites.size(); i++) {
        sprite_distance[i] = tmpSprites[3 - i - 1].first;
        sprite_order[i] = tmpSprites[3 - i - 1].second;
    }

    for (size_t i = 0; i < sprites.size(); i++) {
        // Translate sprite position relative to camera

        float sprite_x = sprites[sprite_order[i]].pos_x - player->pos_x;
        float sprite_y = sprites[sprite_order[i]].pos_y - player->pos_y;

        // Inverse camera matrix

        float inv_det = 1.0f / (player->plane_x * player->dir_y -
                                player->dir_x * player->plane_y);

        // Transform sprite with the inverse camera matrix

        float transform_x =
            inv_det * (player->dir_y * sprite_x - player->dir_x * sprite_y);
        float transform_y = inv_det * (-player->plane_y * sprite_x +
                                       player->plane_x * sprite_y);

        // Sprite screen position

        int32_t sprite_screen_x =
            int32_t((window.width / 2) * (1 + transform_x / transform_y));

        int32_t sprite_height = abs(int32_t(window.height / transform_y));
        int32_t sprite_width = abs(int32_t(window.height / (transform_y)));

        // Information based on sprite height and width

        int32_t draw_start_y = -sprite_height / 2 + window.height / 2;
        if (draw_start_y < 0)
            draw_start_y = 0;

        int32_t draw_end_y = draw_start_y + sprite_height;
        if (draw_end_y > window.height)
            draw_end_y = window.height;

        int32_t draw_start_x = -sprite_width / 2 + sprite_screen_x;
        if (draw_start_x < 0)
            draw_start_x = 0;

        int32_t draw_end_x = sprite_width / 2 + sprite_screen_x;
        if (draw_end_x > window.width)
            draw_end_x = window.width;

        // Iterate through every vertical stripe of the sprite

        for (int32_t stripe = draw_start_x; stripe < draw_end_x; stripe++) {
            int32_t tex_x =
                int32_t(256 * (stripe - (-sprite_width / 2 + sprite_screen_x)) *
                        sprites[sprite_order[i]].getWidth() / sprite_width) /
                256;

            // If the stripe is out of bounds, skip it
            if (transform_y <= 0 || stripe <= 0 || stripe >= window.width ||
                transform_y >= dist_buffer[stripe])
                continue;

            // Render the stripe

            for (int32_t y = draw_start_y; y < draw_end_y; y++) {
                int32_t d = (y)*256 - window.height * 128 + sprite_height * 128;
                int32_t tex_y = ((d * sprites[sprite_order[i]].getHeight()) /
                                 sprite_height) /
                                256;

                int32_t color =
                    sprites[sprite_order[i]].getPixelColor(tex_x, tex_y);
                if (color & 0x00FFFFFF) {
                    glColor3ubv((GLubyte*)&color);
                    glVertex2i(stripe, y);
                }
            }
        }
    }
}

void Game::render(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POINTS);

    renderFloorAndCeiling();
    renderWalls();
    renderSprites();

    glEnd();

    window.swapBuffers();
}

void Game::loop(void) {
    while (!window.shouldClose()) {
        (this->*gameState)();
    }
}

void Game::paused(void) { glfwPollEvents(); }

void Game::update(void) {
    double old_time = time;

    time = glfwGetTime();
    delta_time = time - old_time;

    glfwPollEvents();

    double cursor_pos_x, cursor_pos_y;

    window.getCursorPos(&cursor_pos_x, &cursor_pos_y);
    window.centerCursor();

    (void)cursor_pos_y;

    // Cursor distance from the middle of window

    player->cursor_dir = window.width / 2.0f - cursor_pos_x;

    float move_step = delta_time * player->move_speed * player->walk_dir;
    float side_step = delta_time * player->move_speed * player->side_dir;

    float next_x = player->pos_x + (player->dir_x * move_step) +
                   (player->dir_y * side_step);
    float next_y = player->pos_y + (player->dir_y * move_step) -
                   (player->dir_x * side_step);

    if (map->grid[(size_t)player->pos_y][(size_t)next_x] == '0')
        player->pos_x = next_x;
    if (map->grid[(size_t)next_y][(size_t)player->pos_x] == '0')
        player->pos_y = next_y;

    float rot_step = delta_time * (player->rot_speed * player->rot_dir) +
                     (player->cursor_speed * player->cursor_dir);

    float old_dir_x = player->dir_x;
    player->dir_x =
        player->dir_x * cos(rot_step) - player->dir_y * sin(rot_step);
    player->dir_y = old_dir_x * sin(rot_step) + player->dir_y * cos(rot_step);

    float old_plane_x = player->plane_x;
    player->plane_x =
        player->plane_x * cos(rot_step) - player->plane_y * sin(rot_step);
    player->plane_y =
        old_plane_x * sin(rot_step) + player->plane_y * cos(rot_step);

    render();
}

const char* Game::CouldntOpenMap::what() const throw() {
    return "Couldn't open map";
}

const char* Game::InvalidContent::what() const throw() {
    return "Invalid content";
}

const char* Game::FailedToInitGame::what() const throw() {
    return "Failed to init game";
}
