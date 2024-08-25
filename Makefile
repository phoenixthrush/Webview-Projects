OS=$(shell uname -s)

ifeq ($(OS), Darwin)
	# macOS
	CC=g++
	CFLAGS=-c -std=c++11 -DWEBVIEW_STATIC
	LFLAGS=-framework WebKit
else
ifeq ($(OS), Linux)
	# Linux
	CC=g++
	CFLAGS=-c -std=c++11 -DWEBVIEW_STATIC $(shell pkg-config --cflags gtk+-3.0 webkit2gtk-4.0)
	LFLAGS=$(shell pkg-config --libs gtk+-3.0 webkit2gtk-4.0)
else
	# Windows/MinGW
	CC=g++
	CFLAGS=-c -std=c++14 -DWEBVIEW_STATIC -Ilibs/webview2/build/native/include
	LFLAGS=-mwindows -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion
endif
endif

all:
	mkdir -p build libs/webview

	curl -sSLo "libs/webview/webview.h" "https://raw.githubusercontent.com/webview/webview/master/webview.h"
	curl -sSLo "libs/webview/webview.cc" "https://raw.githubusercontent.com/webview/webview/master/webview.cc"

	xxd -i site/index.html > build/html.c

	$(CC) $(CFLAGS) libs/webview/webview.cc -o build/webview.o
	gcc -c main.c -std=c99 -Ilibs/webview -o build/main.o
	$(CC) build/main.o build/webview.o $(LFLAGS) -o build/main

	./build/main

clean:
	rm -rf build libs
