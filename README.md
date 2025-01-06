# Skeleton emsdk + cmake
Skeleton CMAKE project that downloads and uses Emscripten to compile webassembly projects. There are two projects in the works: clay-app and raygui-app. The first uses the Clay GUI library, but it's not currently working, so it is disabled by default. The second uses raylib + raygui.
```
$ cmake -B build -S .
```
Replace "raygui-app" in the root CMakeLists.txt with your subdirectory containing your C/C++ code.
