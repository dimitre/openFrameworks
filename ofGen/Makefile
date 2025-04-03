#  Still not recompiling when .h files are changed.
# $(info MAKEFILE TEST)

# CXX=/opt/homebrew/opt/llvm/bin/clang++ # define the compiler to use
# # define the compiler to use
CXX=c++
TARGET=ofgen
SOURCES=src/main.cpp src/templates.cpp src/utils.cpp src/addons.cpp src/uuidxx/src/uuidxx.cpp
CXXFLAGS=-std=c++20 -Isrc/uuidxx/src -MMD -MP $(shell pkgconf --cflags yaml-cpp nlohmann_json)
# -Wfatal-errors
# -I../libs/macos/include/
# LDFLAGS=-fuse-ld=lld $(shell pkg-config --libs yaml-cpp nlohmann_json)
LDFLAGS=$(shell pkgconf --libs yaml-cpp nlohmann_json)

################################################################################
### DO NOT EDIT THE FOLLOWING LINES ############################################

# define list of objects
# OBJSC=$(SOURCES:.c=.o)
OBJS=$(SOURCES:.cpp=.o)

# the target is obtained linking all .o files
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

purge: clean
	rm -f $(TARGET)

clean:
	rm -f src/*.o
