CXX = g++
CXXFLAGS = -Wall -g
LDFLAGS = -L/usr/lib -ldiscord-rpc -lglfw
INCLUDES = -I/usr/include

TARGET = EnforcerSyphozia

SRCS = src/main.cpp common/assert.cpp common/discord.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

rebuild: clean all

.PHONY: all clean rebuild
