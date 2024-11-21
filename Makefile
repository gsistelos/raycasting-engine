ifeq ($(OS),Windows_NT)
	TARGET := raycast.exe
else
	TARGET := raycast
endif

SRC := $(addprefix src/, \
	Game.cpp Image.cpp keyCallback.cpp main.cpp Map.cpp \
	Player.cpp Sprite.cpp Window.cpp \
)

OBJ := $(SRC:.cpp=.o)

LIB_DIR := lib

GLFW_DIR := $(LIB_DIR)/glfw

GLFW_BUILD_DIR := $(GLFW_DIR)/build

GLFW := $(GLFW_BUILD_DIR)/src/libglfw3.a

ifeq ($(OS),Windows_NT)
	GLFW_FLAGS := -DCMAKE_TOOLCHAIN_FILE=CMake/x86_64-w64-mingw32.cmake
endif

STB_IMAGE := $(LIB_DIR)/stb_image.h

ifeq ($(OS),Windows_NT)
	CXX := x86_64-w64-mingw32-g++ -O3
else
	CXX := g++ -O3
endif

INCLUDE := -Iinclude -I$(GLFW_DIR)/include -I$(LIB_DIR)

CXXFLAGS := -Wall -Werror -Wextra $(INCLUDE)

ifeq ($(OS),Windows_NT)
	FLAGS := -lgdi32 -lopengl32 -static-libgcc -static-libstdc++
else ifeq ($(shell uname),Linux)
	FLAGS := -lGL
else
	FLAGS := -framework Cocoa -framework OpenGL -framework IOKit
endif

RM := rm -rf

.PHONY: all
all: $(TARGET)

$(TARGET): $(LIB_DIR) $(GLFW) $(STB_IMAGE) $(OBJ)
	$(CXX) $(OBJ) $(GLFW) $(FLAGS) -o $(TARGET)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(GLFW_DIR):
	git clone https://github.com/glfw/glfw $(GLFW_DIR)

$(GLFW_BUILD_DIR): $(GLFW_DIR)
	cmake $(GLFW_DIR) -B $(GLFW_BUILD_DIR) $(GLFW_FLAGS)

$(GLFW): $(GLFW_BUILD_DIR)
	make -C $(GLFW_BUILD_DIR)

$(STB_IMAGE):
	curl -o $(STB_IMAGE) https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

.PHONY: clean
clean:
	$(RM) $(OBJ)

.PHONY: fclean
fclean:
	$(RM) $(OBJ) $(TARGET)

.PHONY: re
re: fclean all
