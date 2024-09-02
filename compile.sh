cd src
xxd -i site/index.html > html.c

cmake -G Ninja -B build -S . -D CMAKE_BUILD_TYPE=Release
cmake --build build

strip ./build/bin/example
du -hs ./build/bin/example

./build/bin/example
cd ..