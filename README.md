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

Run: the executable `sigma_strikers` will look for sprites under
`assets/sprite/`.  Currently two images are used:

* `football_field.jpeg` – stretched to fill the window as the field
  background
* `ball.png` – drawn centred on the puck and scaled to the ball's radius

If the textures are missing the program falls back to a solid-colour field
and a white filled circle for the ball.

## Hockey Field Demo

The application now includes a simple hockey field scaled to the window (40 m × 20 m by
default) with four barrier walls. A white puck moves across the field and
*bounces* off the barriers automatically. This demonstrates basic world-to-screen
scaling and collision handling using a `Field` and `Ball` class.
