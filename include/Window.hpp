#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>

class Window {
  public:
    GLFWwindow* window;
    int32_t width, height;

    Window(void);
    ~Window();

    bool shouldClose(void) const;
    void swapBuffers(void) const;
    void getCursorPos(double* cursor_pos_x, double* cursor_pos_y) const;
    void centerCursor(void) const;

  private:
    const char* _title;
};

#endif /* WINDOW_HPP */
