# Convenience wrappers around CMake.
#
# Usage:
#   make build
#   make run EXPR="(2+5)*2"

BUILD_DIR := build
LAUNCHER := ./toy-app

.PHONY: all configure build run test clean

all: build

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR)

run: build
	@if [ -z "$(EXPR)" ]; then \
		echo "Usage: make run EXPR=\"(2+5)*2\""; \
		exit 2; \
	fi
	@$(LAUNCHER) "$(EXPR)"

test: build
	@ctest --test-dir $(BUILD_DIR) --output-on-failure

clean:
	rm -rf $(BUILD_DIR)
