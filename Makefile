NAME		=	raycast

SRCS		=	$(addprefix srcs/, main.cpp Game.cpp Image.cpp keyCallback.cpp Map.cpp \
				Player.cpp Sprite.cpp)

OBJS		=	$(SRCS:.cpp=.o)

CXX			=	g++ -O3

CXXFLAGS	=	-Wall -Werror -Wextra

GLFW_DIR	=	glfw/

GLFW_BUILD	=	$(GLFW_DIR)build/

GLFW_FLAGS	=	

GLFW		=	$(GLFW_BUILD)src/libglfw3.a

STB_IMAGE	=	includes/stb_image.h

INCLUDES	=	-Iincludes/ -I$(GLFW_DIR)include/

FLAGS		=	-lm -lGL

RM			=	rm -f

all: $(NAME)

windows: set_windows all

set_windows:
	$(eval CXX=x86_64-w64-mingw32-g++ -O3)
	$(eval GLFW_FLAGS=-D CMAKE_TOOLCHAIN_FILE=CMake/x86_64-w64-mingw32.cmake)
	$(eval FLAGS=-lgdi32 -lopengl32 -static-libgcc -static-libstdc++)

$(NAME): $(STB_IMAGE) $(GLFW) $(OBJS)
	$(CXX) $(OBJS) $(GLFW) $(FLAGS) -o $(NAME)

$(STB_IMAGE):
	wget -P includes/ https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

$(GLFW):
	cmake $(GLFW_DIR) -B $(GLFW_BUILD) $(GLFW_FLAGS)
	make -s -C $(GLFW_BUILD)

.cpp.o:
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(STB_IMAGE) $(OBJS)
	$(RM) -r $(GLFW_BUILD)

fclean:
	$(RM) $(STB_IMAGE) $(OBJS) $(NAME)
	$(RM) -r $(GLFW_BUILD)

re: fclean all

.PHONY: all clean fclean re
