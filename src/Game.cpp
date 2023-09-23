#include "Game.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

void keyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods);

Game::Game(void) : windowWidth(720), windowHeight(480), player(deltaTime, map) {
}

Game::~Game() {
    glfwTerminate();
}

void Game::init(const std::string& filename) {
    loadTexturesAndSprites(filename);

    map.load(filename);
    player.load();

    if (glfwInit() == GLFW_FALSE)
        throw Game::FailedToInitGame();

    window = glfwCreateWindow(windowWidth, windowHeight, "Raycast", nullptr, nullptr);
    if (window == nullptr)
        throw Game::FailedToInitGame();

    glfwMakeContextCurrent(window);
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
    glfwSetWindowSizeLimits(window, windowWidth, windowHeight, windowWidth, windowHeight);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    gameState = &Game::update;

    time = glfwGetTime();
}

void Game::loadTexturesAndSprites(const std::string& filename) {
    std::ifstream mapFile(filename);
    if (mapFile.is_open() == false)
        throw Game::CouldntOpenMap();

    std::string line;
    while (std::getline(mapFile, line, '\n')) {
        if (line.empty())
            continue;

        if (line == "MAP")
            break;

        if (line.length() < 3)
            throw Game::InvalidContent();

        if (!isspace(line[0]) && line[1] == ' ') {
            textures[line[0]] = Image(line.substr(2));
        } else {
            // Handle errors for each sprite coordinate

            int32_t pos[2];
            for (int32_t ipos = 0; ipos < 2; ipos++) {
                size_t end = line.find(' ');
                if (end == std::string::npos || end > 3)
                    throw Game::InvalidContent();

                for (size_t i = 0; i < end; i++) {
                    if (!isdigit(line[i]))
                        throw Game::InvalidContent();
                }

                pos[ipos] = std::atoi(line.substr(0, end).c_str());

                line.erase(0, end + 1);
            }

            sprites.push_back(Sprite(pos[0], pos[1], line.substr()));
        }
    }

    mapFile.close();

    // Vectors for sprite rendering calculations

    distBuffer.resize(windowWidth);
    spriteOrder.resize(sprites.size());
    spriteDistance.resize(sprites.size());
}

void Game::renderWalls(void) {
    for (int32_t x = 0; x < windowWidth; x++) {
        // Ray direction in current iteration

        float cameraX = 2 * x / float(windowWidth) - 1;

        float rayDirX = player.dirX + player.planeX * cameraX;
        float rayDirY = player.dirY + player.planeY * cameraX;

        // Reset mapX and mapY each iteration

        int32_t mapX = player.posX;
        int32_t mapY = player.posY;

        // Distance for each step

        float deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
        float deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);

        // Closest interception distance and which direction to step

        float sideDistX, sideDistY;
        int32_t stepX, stepY;
        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (player.posX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0f - player.posX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (player.posY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0f - player.posY) * deltaDistY;
        }

        // Step until find a wall

        bool side;
        bool hit = 0;
        while (hit == 0) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if (map.grid[mapY][mapX] != '0')
                hit = 1;
        }

        // Store the hit distance

        if (side == 0)
            distBuffer[x] = (sideDistX - deltaDistX);
        else
            distBuffer[x] = (sideDistY - deltaDistY);

        // Draw information based on distance

        int32_t lineHeight = windowHeight / distBuffer[x];

        int32_t drawStart = windowHeight / 2 - lineHeight / 2;
        if (drawStart < 0)
            drawStart = 0;

        int32_t drawEnd = drawStart + lineHeight;
        if (drawEnd > windowHeight)
            drawEnd = windowHeight;

        // Position where the wall was hit

        float wallX;
        if (side == 0)
            wallX = player.posY + distBuffer[x] * rayDirY;
        else
            wallX = player.posX + distBuffer[x] * rayDirX;
        wallX -= (int32_t)wallX;

        // Wall texture

        Image& texture = textures[map.grid[mapY][mapX]];

        // Convert the wallX position into texX position

        int32_t texX = wallX * texture.getWidth();
        if (side == 0 && rayDirX > 0)
            texX = texture.getWidth() - texX - 1;
        if (side == 1 && rayDirY < 0)
            texX = texture.getWidth() - texX - 1;

        // Start and step for texY

        float step = 1.0f * texture.getHeight() / lineHeight;
        float texPos = (drawStart - windowHeight / 2 + lineHeight / 2) * step;

        // Render a vertical line

        for (int32_t y = drawStart; y < drawEnd; y++) {
            int32_t texY = (int32_t)texPos & (texture.getHeight() - 1);
            texPos += step;

            int32_t color = texture.getPixelColor(texX, texY);
            if (side == 1) color = (color >> 1) & 8355711;

            glColor3ubv((GLubyte*)&color);
            glVertex2i(x, y);
        }
    }
}

void Game::renderFloorAndCeiling(void) {
    // Floor and ceiling textures

    Image& floorTexture = textures['F'];
    Image& ceilingTexture = textures['C'];

    for (int32_t y = windowHeight / 2; y < windowHeight; y++) {
        // Ray direction for the floor

        float rayDirX0 = player.dirX - player.planeX;
        float rayDirY0 = player.dirY - player.planeY;
        float rayDirX1 = player.dirX + player.planeX;
        float rayDirY1 = player.dirY + player.planeY;

        // Horizontal distance from the camera to the floor for the current row

        float rowDistance = 0.5f * windowHeight / (y - windowHeight / 2);

        // Starting position of the floor

        float floorX = player.posX + rowDistance * rayDirX0;
        float floorY = player.posY + rowDistance * rayDirY0;

        // How much to step from one pixel to the next

        float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / windowWidth;
        float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / windowWidth;

        for (int32_t x = 0; x < windowWidth; x++) {
            int32_t cellX = (int32_t)(floorX);
            int32_t cellY = (int32_t)(floorY);

            int32_t texX = (int32_t)(floorTexture.getWidth() * (floorX - cellX)) & (floorTexture.getWidth() - 1);
            int32_t texY = (int32_t)(floorTexture.getHeight() * (floorY - cellY)) & (floorTexture.getHeight() - 1);

            floorX += floorStepX;
            floorY += floorStepY;

            // Render floor and ceiling

            int32_t color = floorTexture.getPixelColor(texX, texY);
            color = (color >> 1) & 8355711;
            glColor3ubv((GLubyte*)&color);
            glVertex2i(x, y);

            color = ceilingTexture.getPixelColor(texX, texY);
            color = (color >> 1) & 8355711;
            glColor3ubv((GLubyte*)&color);
            glVertex2i(x, windowHeight - y - 1);
        }
    }
}

void Game::renderSprites(void) {
    // TODO: refactor sprite sorting

    for (size_t i = 0; i < sprites.size(); i++) {
        spriteOrder[i] = i;
        spriteDistance[i] = ((player.posX - sprites[i].posX) * (player.posX - sprites[i].posX) + (player.posY - sprites[i].posY) * (player.posY - sprites[i].posY));
    }

    std::vector<std::pair<double, int>> tmpSprites(sprites.size());
    for (size_t i = 0; i < sprites.size(); i++) {
        tmpSprites[i].first = spriteDistance[i];
        tmpSprites[i].second = spriteOrder[i];
    }
    std::sort(tmpSprites.begin(), tmpSprites.end());
    for (size_t i = 0; i < sprites.size(); i++) {
        spriteDistance[i] = tmpSprites[3 - i - 1].first;
        spriteOrder[i] = tmpSprites[3 - i - 1].second;
    }

    for (size_t i = 0; i < sprites.size(); i++) {
        // Translate sprite position relative to camera

        float spriteX = sprites[spriteOrder[i]].posX - player.posX;
        float spriteY = sprites[spriteOrder[i]].posY - player.posY;

        // Inverse camera matrix

        float invDet = 1.0f / (player.planeX * player.dirY - player.dirX * player.planeY);

        // Transform sprite with the inverse camera matrix

        float transformX = invDet * (player.dirY * spriteX - player.dirX * spriteY);
        float transformY = invDet * (-player.planeY * spriteX + player.planeX * spriteY);

        // Sprite screen position

        int32_t spriteScreenX = int32_t((windowWidth / 2) * (1 + transformX / transformY));

        int32_t spriteHeight = abs(int32_t(windowHeight / transformY));
        int32_t spriteWidth = abs(int32_t(windowHeight / (transformY)));

        // Draw information based on sprite height and width

        int32_t drawStartY = -spriteHeight / 2 + windowHeight / 2;
        if (drawStartY < 0)
            drawStartY = 0;

        int32_t drawEndY = drawStartY + spriteHeight;
        if (drawEndY > windowHeight)
            drawEndY = windowHeight;

        int32_t drawStartX = -spriteWidth / 2 + spriteScreenX;
        if (drawStartX < 0)
            drawStartX = 0;

        int32_t drawEndX = spriteWidth / 2 + spriteScreenX;
        if (drawEndX > windowWidth)
            drawEndX = windowWidth;

        for (int32_t stripe = drawStartX; stripe < drawEndX; stripe++) {
            int32_t texX = int32_t(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * sprites[spriteOrder[i]].texture.getWidth() / spriteWidth) / 256;

            // If the sprite is out of bounds, skip it
            if (transformY <= 0 || stripe <= 0 || stripe >= windowWidth || transformY >= distBuffer[stripe])
                continue;

            // Render the sprite vertical stripe

            for (int32_t y = drawStartY; y < drawEndY; y++) {
                int32_t d = (y)*256 - windowHeight * 128 + spriteHeight * 128;
                int32_t texY = ((d * sprites[spriteOrder[i]].texture.getHeight()) / spriteHeight) / 256;

                int32_t color = sprites[spriteOrder[i]].texture.getPixelColor(texX, texY);
                if (color & 0x00FFFFFF) {
                    glColor3ubv((GLubyte*)&color);
                    glVertex2i(stripe, y);
                }
            }
        }
    }
}

void Game::paused(void) {
    glfwPollEvents();
}

void Game::update(void) {
    double oldTime = time;
    time = glfwGetTime();
    deltaTime = time - oldTime;

    glfwPollEvents();

    double cursorPosX, cursorPosY;

    glfwGetCursorPos(window, &cursorPosX, &cursorPosY);
    glfwSetCursorPos(window, windowWidth / 2.0, windowHeight / 2.0);

    (void)cursorPosY;

    // Cursor distance from the middle of window

    player.cursorDir = windowWidth / 2.0f - cursorPosX;

    player.update();

    render();
}

void Game::render(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POINTS);

    renderFloorAndCeiling();
    renderWalls();
    renderSprites();

    glEnd();

    glfwSwapBuffers(window);
}

void Game::loop(void) {
    while (!glfwWindowShouldClose(window)) {
        (this->*gameState)();
    }
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
