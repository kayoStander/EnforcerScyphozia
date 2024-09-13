CXX = g++
CXXFLAGS = -Wall -g -std=c++23
LDFLAGS = -L/usr/lib -ldiscord-rpc -lglfw -lfmt
INCLUDES = -I/usr/include -Icommon

TARGET = EnforcerSyphozia

SRCS = $(wildcard src/*.cpp common/*.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	# rm -f $(TARGET) $(OBJS)

rebuild: clean all

.PHONY: all clean rebuild
