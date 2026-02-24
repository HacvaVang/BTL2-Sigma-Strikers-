cmake_minimum_required(VERSION 3.10)
project(SigmaStrikersSDL)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} include)

add_executable(sigma_strikers
    src/main.cpp
    src/SDLFramework.cpp
    src/Menu.cpp
    src/Field.cpp
    src/Ball.cpp
)

# copy asset directory so the executable can load sprites/textures
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# target_link_libraries(sigma_strikers ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(sigma_strikers PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)