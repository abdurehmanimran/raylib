NAME := grid
SRC_FILES := $(wildcard src/*)
INCLUDE_DIR := include
LIB_DIR := lib
LIB_FLAGS := lraylib -lGL -lm -lpthread -ldl -lrt -lX11

BUILD_DIR := bin

BIN_PATH := ${BUILD_DIR}/${NAME}

build ${BIN_PATH}: ${SRC_FILES}
	mkdir -p ${BUILD_DIR}
	g++ -I${INCLUDE_DIR} -L${LIB_DIR} ${SRC_FILES} -${LIB_FLAGS} -o ${BIN_PATH}

run : ${BIN_PATH}
	${BIN_PATH}
