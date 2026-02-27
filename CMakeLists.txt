cmake_minimum_required(VERSION 3.10)
project(SigmaStrikersSDL)

# Thiết lập chuẩn C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tắt tự động copy DLL của vcpkg (để tránh lỗi trên Linux)
set(VCPKG_APPLOCAL_DEPS OFF)

# Tìm các gói thư viện cần thiết
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Thư mục include
include_directories(${SDL2_INCLUDE_DIRS} include)

# Định nghĩa file thực thi
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

# Liên kết các thư viện
target_link_libraries(sigma_strikers PRIVATE 
    SDL2::SDL2main 
    SDL2::SDL2 
    SDL2_image::SDL2_image 
    SDL2_ttf::SDL2_ttf
)

# 1. Copy thư mục assets vào thư mục build để game có thể load ảnh/font
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# 2. Xử lý các file DLL (Chỉ chạy khi build trên WINDOWS)
if(WIN32)
    message(STATUS "Detected Windows environment - Configuring DLL copy commands...")

    # Copy vcpkg runtime DLLs
    file(GLOB VCPKG_DLLS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll")
    if(VCPKG_DLLS)
        add_custom_command(TARGET sigma_strikers POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${VCPKG_DLLS}
                $<TARGET_FILE_DIR:sigma_strikers>
            COMMENT "Copying vcpkg runtime DLLs to output directory"
        )
    endif()

    # Copy MinGW runtime DLLs (Dành cho trình biên dịch GCC trên Windows)
    get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
    set(MINGW_DLLS
        "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
        "${MINGW_BIN_DIR}/libstdc++-6.dll"
        "${MINGW_BIN_DIR}/libwinpthread-1.dll"
    )
    
    # Kiểm tra xem file có tồn tại thực sự không trước khi tạo lệnh copy
    foreach(DLL ${MINGW_DLLS})
        if(EXISTS ${DLL})
            add_custom_command(TARGET sigma_strikers POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${DLL}
                    $<TARGET_FILE_DIR:sigma_strikers>
                COMMENT "Copying MinGW runtime DLL: ${DLL}"
            )
        endif()
    endforeach()
else()
    message(STATUS "Detected Linux/WSL environment - Skipping DLL copy.")
endif()