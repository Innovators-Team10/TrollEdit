mkdir _build
cd _build

cmake .. -DCMAKE_INSTALL_PREFIX=../TrollEdit -G "MinGW Makefiles" %*
cmake --build . --target install
