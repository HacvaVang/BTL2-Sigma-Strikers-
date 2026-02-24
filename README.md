# Sigma Strikers — SDL Framework

Small C++ SDL2 starter framework with a render loop, input handling, and optional sprite loading.

Build (recommended):

- Install SDL2 and SDL2_image on your system or via vcpkg.
- Example (using CMake):

```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

Run: the executable `sigma_strikers` will look for `assets/sprite.png`.

If you don't have a sprite, the demo will render a placeholder rectangle.
