# Makefile for a C++ project with main.cpp and tools.cpp

CC := g++-13
CXX := em++
# CFLAGS를 두 가지 모드로 정의합니다.
# -g: 디버깅 정보 포함
# -O0: 최적화 끔 (디버깅 시 코드 흐름 이해에 용이)
# Release 모드 (기본)
CFLAGS_RELEASE := -Wall -Wextra -std=c++17 -O2

# Debug 모드
CFLAGS_DEBUG := -Wall -Wextra -std=c++17 -g -O0

# JS 모드
CFLAGS_JS = -Wall -std=c++17 -I./include -s WASM=1 -s MODULARIZE=1 -s EXPORT_ES6=1 -O3

EXPORTED_FUNCTIONS = "['_solveMap', '_freeMemory']"
EM_FLAGS = -s EXPORTED_FUNCTIONS=$(EXPORTED_FUNCTIONS)

# 기본적으로 Release 모드를 사용하도록 설정
# make debug 를 실행하면 DEBUG=1이 되고 CFLAGS가 CFLAGS_DEBUG로 설정됩니다.
ifeq ($(DEBUG), 1)
    CFLAGS := $(CFLAGS_DEBUG)
else
    CFLAGS := $(CFLAGS_RELEASE)
endif

SRCS := $(wildcard ./drag_manage/*.cpp)
OBJS := $(SRCS:.cpp=.o)
TARGET := app

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) -f $(TARGET).js $*.wasm

# 디버그 빌드를 위한 특별한 타겟
debug:
	$(MAKE) DEBUG=1 all

js: $(TARGET).js

$(TARGET).js: $(SRCS)
	$(CXX) $(CFLAGS_JS) $(EM_FLAGS) $(SRCS) -o $(TARGET).js
	@echo "JavaScript build complete: $(TARGET).js"
.PHONY: all clean debug js