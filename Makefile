TARGET := raycast.exe

SRC := $(addprefix src/,	Game.cpp Image.cpp keyCallback.cpp main.cpp Map.cpp \
							Player.cpp Sprite.cpp \
							)

OBJ := $(SRC:.cpp=.o)

GLFW_DIR := glfw

GLFW_FLAGS := -DCMAKE_TOOLCHAIN_FILE=CMake/x86_64-w64-mingw32.cmake

GLFW := glfw/build/src/libglfw3.a

STB_IMAGE := include/stb_image.h

CXX := x86_64-w64-mingw32-g++ -O3

CXXFLAGS := -Wall -Werror -Wextra -Iinclude -I$(GLFW_DIR)/include

FLAGS := -lgdi32 -lopengl32 -static-libgcc -static-libstdc++

RM := rm -rf

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
	$(RM) $(OBJ) $(TARGET) $(GLFW_DIR)/build $(STB_IMAGE)

re: clean all

.PHONY: all clean re
