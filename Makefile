#for sdl application

vpath %.h include
vpath %.hpp include

LIBPATH=-lSDL2 -lGL -lGLU -lGLEW -lIL -lILU -lassimp
CXX=clang++
CC=clang
#CFLAGS=-Wall -Werror 
CFLAGS=-Wall 
#CXXFLAGS=-Wextra -std=c++11 -Wall -Werror 
CXXFLAGS=-Wextra -std=c++11 -Wall -g -O0  
CPPFLAGS=-DGLEW_STATIC -DDEBUG
#program name
PROG=build/prog
#source codes
SRCS_C=$(wildcard src/*.c)
SRCS_CPP=$(wildcard src/*.cpp)
OBJS_C=$(patsubst src/%.c,build/%.o,$(SRCS_C) )
OBJS_CPP=$(patsubst src/%.cpp,build/%.o,$(SRCS_CPP) )
OBJS=$(OBJS_C) $(OBJS_CPP)
DEPS_C=$(patsubst src/%.c,build/%.d,$(SRCS_C) )
DEPS_CPP=$(patsubst src/%.cpp,build/%.d,$(SRCS_CPP) )
DEPS=$(DEPS_C) $(DEPS_CPP)

.PHONY: all clean
all: $(PROG) 
	@echo Make Complete!
-include $(DEPS)
$(PROG): $(OBJS)
	$(CXX) -o $@ $(LDFLAGS) $^ $(LIBPATH)
build/%.o: src/%.cpp 
	$(CXX) -MMD -MP -MF $(patsubst src/%.cpp,build/%.d,$<) -o $@ -c $(CXXFLAGS) $(CPPFLAGS) $<
clean:
	$(RM) build/*.d build/*.o $(PROG)

