TrollEdit - Graphically Enhanced Text Editor
=========

[![Build Status](https://secure.travis-ci.org/Innovators-Team10/TrollEdit.png?branch=master)](http://travis-ci.org/Innovators-Team10/TrollEdit)

TrollEdit is an ongoing experiment to create an usable graphically enhanced text editor for programming. Instead of providing simple syntax highlighting TrollEdit utilized PEG grammars and abstract syntactic trees for visualization.

Build
-----

Windows MinGW

```bash
mkdir _build
cd _build
cmake .. -G"MinGW Makefiles" -DQT_QMAKE_EXECUTABLE=[path to qmake]
cmake --build . --target install
```

Linux/Unix/OSX

```bash
mkdir _build
cd _build
cmake .. -G"Unix Makefiles" -DQT_QMAKE_EXECUTABLE=[path to qmake]
cmake --build . --target install
```

Apple Bundle

```bash
mkdir _build
cd _build
cmake .. -G"Unix Makefiles" -DQT_QMAKE_EXECUTABLE=[path to qmake] -DTROLLEDIT_BUILD_BUNDLE=ON
cmake --build . --target install
```

The result will be in the `TrollEdit` directory of the source.
