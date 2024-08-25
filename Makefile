OS := $(shell uname)

CC = gcc
CCP = g++

ifeq ($(OS), Darwin)
    # macOS
    CFLAGS = -std=c99 -DWEBVIEW_STATIC -I/opt/homebrew/opt/openssl@3/include
    CXXFLAGS = -std=c++11 -DWEBVIEW_STATIC -I/opt/homebrew/opt/openssl@3/include
    LFLAGS = -framework WebKit -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto
else ifeq ($(OS), Linux)
    # Linux
    CFLAGS = -std=c99 -DWEBVIEW_STATIC -I/usr/local/opt/openssl/include $(shell pkg-config --cflags gtk+-3.0 webkit2gtk-4.0)
    CXXFLAGS = -std=c++11 -DWEBVIEW_STATIC -I/usr/local/opt/openssl/include $(shell pkg-config --cflags gtk+-3.0 webkit2gtk-4.0)
    LFLAGS = $(shell pkg-config --libs gtk+-3.0 webkit2gtk-4.0) -L/usr/local/opt/openssl/lib -lssl -lcrypto
else
    # Windows/MinGW
    CFLAGS = -std=c99 -DWEBVIEW_STATIC -Ilibs/webview2/build/native/include -IC:\\Program Files\\OpenSSL-Win64\\include
    CXXFLAGS = -std=c++14 -DWEBVIEW_STATIC -Ilibs/webview2/build/native/include -IC:\\Program Files\\OpenSSL-Win64\\include
    LFLAGS = -mwindows -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion -LC:\\Program Files\\OpenSSL-Win64\\lib -lssl -lcrypto
endif

# Default target
all: build/main

# Create build and libs/webview directories
build:
	mkdir -p build
	mkdir -p libs/webview

# Download webview header and source
libs/webview/webview.h libs/webview/webview.cc: | build
	curl -sSLo "libs/webview/webview.h" "https://raw.githubusercontent.com/webview/webview/master/webview.h"
	curl -sSLo "libs/webview/webview.cc" "https://raw.githubusercontent.com/webview/webview/master/webview.cc"

# Convert index.html to build/html.c
build/html.c: site/index.html
	xxd -i site/index.html > build/html.c

# Build the main executable
build/main: build build/html.c libs/webview/webview.h libs/webview/webview.cc build/main.o build/webview.o build/xor_encrypt.o
	$(CCP) build/main.o build/webview.o build/xor_encrypt.o $(LFLAGS) -o build/main

# Compile main.c to build/main.o
build/main.o: src/main.c build/html.c libs/webview/webview.h
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

# Compile webview.cc to build/webview.o
build/webview.o: libs/webview/webview.cc libs/webview/webview.h
	$(CCP) $(CXXFLAGS) -c libs/webview/webview.cc -o build/webview.o

# Compile xor_encrypt.c to build/xor_encrypt.o
build/xor_encrypt.o: src/xor_encrypt.c
	$(CC) $(CFLAGS) -c src/xor_encrypt.c -o build/xor_encrypt.o

# Clean up build files and libs
clean:
	rm -rf build libs
