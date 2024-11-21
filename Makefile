ifeq ($(OS),Windows_NT)
	TARGET = raycast.exe
else
	TARGET = raycast
endif

SRC = $(addprefix src/, \
	Game.cpp Image.cpp keyCallback.cpp main.cpp Map.cpp \
	Player.cpp Sprite.cpp Window.cpp \
)

OBJ = $(SRC:.cpp=.o)

GLFW_DIR = glfw

ifeq ($(OS),Windows_NT)
	GLFW_FLAGS = -DCMAKE_TOOLCHAIN_FILE=CMake/x86_64-w64-mingw32.cmake
endif

GLFW = glfw/build/src/libglfw3.a

STB_IMAGE = include/stb_image.h

ifeq ($(OS),Windows_NT)
	CXX = x86_64-w64-mingw32-g++ -O3
else
	CXX = g++ -O3
endif

CXXFLAGS = -Wall -Werror -Wextra -Iinclude -I$(GLFW_DIR)/include

ifeq ($(OS),Windows_NT)
	FLAGS = -lgdi32 -lopengl32 -static-libgcc -static-libstdc++
else ifeq ($(shell uname),Linux)
	FLAGS = -lGL
else
	FLAGS = -framework Cocoa -framework OpenGL -framework IOKit
endif

RM = rm -rf

all: $(TARGET)

$(TARGET): $(GLFW) $(STB_IMAGE) $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ) $(GLFW) $(FLAGS)

$(GLFW):
	cmake $(GLFW_DIR) -B $(GLFW_DIR)/build $(GLFW_FLAGS)
	make -C $(GLFW_DIR)/build

$(STB_IMAGE):
	curl -o $(STB_IMAGE) https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	$(RM) $(OBJ)

fclean:
	$(RM) $(OBJ) $(TARGET)

re: fclean all

.PHONY: all clean fclean re
