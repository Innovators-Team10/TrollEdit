<pre>                           <img  src="http://innovators-team10.github.com/images/trolledit.png" align="middle" alt="logo"></img></pre>
 
TrollEdit - Graphically Enhanced Text Editor
-----
[![Build Status](https://secure.travis-ci.org/Innovators-Team10/TrollEdit.png?branch=dev)](http://travis-ci.org/Innovators-Team10/TrollEdit)

TrollEdit is a Qt based text editor developed by students at Slovak University of Technology.

The main goal of the project is to teach students open source deveopmnet and team management.
TrollEdit itself is an experiment to utilize full AST code analysis during writing of code.
This can have benefits in various common tasks such as moving blocks of code, syntax checking,
syntax highlighting etc.

This repository contains a fork of the project which I modified to use CMake based build system 
instead of the default qmake. Other modifications include statically linked in lpeg and reordered
project file structure.

Build
-----

Before you try to install from source make sure you have all submodules of the repository cloned. This is usually done using `git submodule update --init`

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

The result will be in the `TrollEdit` directory of the source. On Apple the result is an application bundle.

Repo Contents
-----
* `data` -          contents grammars for programming language and CSS configuration file for design of TrollEdit
* `dependencies` -  contents git submodules for libraries LPeg, LuaJIT
* `res` -           contents icons and images for user controls 
* `src` -           contents source code (.h, .cpp)
* `ui` -            contents user interface for widgets 
* `.gitmodules` -   file for loading git submodules 
* `.travis.yml` -   script file for CI
* `CmakeList.txt` - script file for build of program 

Links
-----

Slovak University of Technology:
http://www.fiit.stuba.sk/generate_page.php?page_id=749

Project page: 
http://innovators-team10.github.com

Detailed Install instructions:
https://github.com/Innovators-Team10/TrollEdit/wiki/Install-instructions

Contact
-------

tp-team-10@googlegroups.com 
