CXX ?= c++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS := -Iinclude
LDLIBS := -pthread

.PHONY: all run test clean

all: build/thread_pool_demo build/thread_pool_test

build/thread_pool_demo: src/main.cpp include/thread_pool.hpp
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/main.cpp -o $@ $(LDLIBS)

build/thread_pool_test: tests/thread_pool_test.cpp include/thread_pool.hpp
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/thread_pool_test.cpp -o $@ $(LDLIBS)

run: build/thread_pool_demo
	./build/thread_pool_demo

test: build/thread_pool_test
	./build/thread_pool_test

clean:
	rm -rf build
