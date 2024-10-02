CXX = g++
DEFINES = -DDEBUG
CXXFLAGS = -Wall -Wextra -Werror -Wsign-conversion -Wstrict-overflow=2 -mcmodel=small -Wvla -O3 -std=c++23 
LDFLAGS = -L/usr/lib -ldiscord-rpc -lglfw -lfmt -lvulkan 
INCLUDES = -I/usr/include -Icommon -Icommon/tiny_obj_loader.hpp

TARGET = EnforcerSyphozia

SRCS = $(wildcard src/*.cpp src/systems/*.cpp common/*.cpp common/logging/*.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS) 
	./compile.sh
	$(CXX) $(DEFINES) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(DEFINES) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

rebuild: clean all

.PHONY: all clean rebuild
