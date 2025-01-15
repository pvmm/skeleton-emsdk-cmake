# Skeleton emsdk + cmake + raylib

Skeleton CMAKE project that downloads and uses Emscripten to compile raylib webassembly projects. There are two projects in the works: clay-app and raygui-app. The first uses the [Clay](https://github.com/nicbarker/clay) Layout library, but it's not currently working, so it is disabled by default. The second uses [Raygui](https://github.com/raysan5/raygui) GUI library.
```
cmake -B build -S .
```
Replace `raygui-app` in the root `CMakeLists.txt` with your subdirectory containing your C/C++ code.

# Running raygui-app compiled to webassembly

You can access the latest version [here](https://pvmm.github.io/skeleton-emsdk-cmake/) or you can execute raygui-app locally, but you need a http server. Just execute this inside the `build` directory:
```
python -m http.server 8080
```
and point the browser to `localhost:8080`
