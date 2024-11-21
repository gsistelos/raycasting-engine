#include "Game.hpp"

extern Game game;

void keyCallback(GLFWwindow* window, int32_t key, int32_t scancode,
                 int32_t action, int32_t mods) {
    (void)scancode;
    (void)mods;

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            if (game.gameState == &Game::update) {
                game.gameState = &Game::paused;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else {
                game.gameState = &Game::update;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                glfwSetCursorPos(window, game.window.width / 2.0,
                                 game.window.height / 2.0);
                game.time = glfwGetTime();
            }
            break;
        case GLFW_KEY_Q:
            exit(0);
            break;
        case GLFW_KEY_W:
            game.player->walk_dir++;
            break;
        case GLFW_KEY_S:
            game.player->walk_dir--;
            break;
        case GLFW_KEY_A:
            game.player->side_dir--;
            break;
        case GLFW_KEY_D:
            game.player->side_dir++;
            break;
        case GLFW_KEY_LEFT:
            game.player->rot_dir++;
            break;
        case GLFW_KEY_RIGHT:
            game.player->rot_dir--;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_W:
            game.player->walk_dir--;
            break;
        case GLFW_KEY_S:
            game.player->walk_dir++;
            break;
        case GLFW_KEY_A:
            game.player->side_dir++;
            break;
        case GLFW_KEY_D:
            game.player->side_dir--;
            break;
        case GLFW_KEY_LEFT:
            game.player->rot_dir--;
            break;
        case GLFW_KEY_RIGHT:
            game.player->rot_dir++;
        }
    }
}
