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
    src/Team.cpp
    src/Field.cpp
    src/Ball.cpp
    src/AIAgent.cpp
    src/HUD.cpp
)

# copy asset directory so the executable can load sprites/textures
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# target_link_libraries(sigma_strikers ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(sigma_strikers PRIVATE SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)

# automatically copy all runtime DLLs from vcpkg next to the executable after build
file(GLOB VCPKG_DLLS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll")
add_custom_command(TARGET sigma_strikers POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${VCPKG_DLLS}
        $<TARGET_FILE_DIR:sigma_strikers>
    COMMENT "Copying vcpkg runtime DLLs to output directory"
)

# copy MinGW runtime DLLs (libgcc, libstdc++, libwinpthread)
get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
set(MINGW_DLLS
    "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
    "${MINGW_BIN_DIR}/libstdc++-6.dll"
    "${MINGW_BIN_DIR}/libwinpthread-1.dll"
)
add_custom_command(TARGET sigma_strikers POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${MINGW_DLLS}
        $<TARGET_FILE_DIR:sigma_strikers>
    COMMENT "Copying MinGW runtime DLLs to output directory"
)