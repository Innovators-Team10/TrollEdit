mkdir _build
cd _build

cmake .. -DLUA_INCLUDE_DIR=lua -DLUA_LIBRARY=lua/lua5.1.dll -DCMAKE_INSTALL_PREFIX=../TrollEdit -G "MinGW Makefiles" %*
cmake --build . --target install
