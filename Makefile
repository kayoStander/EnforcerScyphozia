CXX = g++
DEFINES = -DDEBUG
CXXFLAGS = -Wall -Wextra -Werror -Wno-sign-conversion -Wstrict-overflow=2 -mcmodel=small -Wvla -O3 -std=c++23
LDFLAGS = -L/usr/lib -ldiscord-rpc -lglfw -lfmt -lvulkan 
INCLUDES = -I/usr/include -Icommon -Iexternal -Iexternal/imgui

TARGET = EnforcerSyphozia

IMGUI_SRCS = $(wildcard external/imgui/*.cpp) $(wildcard external/imgui/backends/imgui_impl_vulkan.cpp) $(wildcard external/imgui/backends/imgui_impl_glfw.cpp)
SRCS = $(wildcard src/*.cpp src/systems/*.cpp common/*.cpp common/logging/*.cpp) $(IMGUI_SRCS)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS) 
	./compile.sh
	$(CXX) $(DEFINES) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(DEFINES) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

install:
	@if [ ! -d "external/imgui" ]; then \
		git clone https://github.com/ocornut/imgui external/imgui; \ # prog smarter to save it in /usr/include but whatever
	else \
		echo "ImGui already installed."; \
	fi


rebuild: clean all

.PHONY: all clean rebuild
