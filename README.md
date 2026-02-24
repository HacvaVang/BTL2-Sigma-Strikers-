# Sigma Strikers — SDL Framework

Small C++ SDL2 starter framework with a render loop, input handling, and optional sprite loading.

## Building the Project ✅

The easiest way to build is with CMake (tested on Windows).
Follow these steps from a PowerShell prompt:

```powershell
# from the repository root
office> mkdir build          # create an out-of-source folder
office> cd build
office> cmake ..               # configure; vcpkg users may wish to pass
                              # -DCMAKE_TOOLCHAIN_FILE="<path>\vcpkg\scripts\buildsystems\vcpkg.cmake"
office> cmake --build .      # build default configuration (Debug/Release)
```

CMake will automatically find SDL2/SDL2_image if they are installed system-wide or
if you add them via vcpkg.  You can also build with `cmake --build . --config Release`
for an optimized build.

The resulting executable (`sigma_strikers` or `sigma_strikers.exe` on Windows) will
look for `assets/sprite.png` in the working directory.  If the file is missing a
simple white circle placeholder is drawn.

> **Tip:** rerun `cmake ..` after changing compiler options or switching between
> Debug/Release to update the project files.



# TO DO LIST

## High Priority
- [ ] Tạo nhân vật có thể di chuyển trong các vùng được phép (Không được đi ra ngoài 4 khung và đi xuyên chướng ngại)
- [ ] Tạo bảng UI hiện điểm và thời gian đơn giản
- [ ] Tạo đĩa hockey có thể tương tác được vật thể trên sân và 4 khung trên sân
- [ ] Tạo chức năng đổi giữa 2 nhân vật
- [ ] Tạo chức năng 2 người chơi (PvP)
- [ ] Xây dựng AI Agent đơn giản cho nhân vật phụ (Nhân vật không được điều khiển bởi người chơi)
- [ ] Tạo Menu Setting 

## Medium Priority
- [ ] Xây dựng kĩ năng nhân vật
- [ ] Tạo effect nhân vật
- [ ] Xây dựng UI đơn giản hiển thị nhân vật, skill
- [ ] Thêm Animation cho skill
- [ ] Thêm Animation cho UI
- [ ] Hiển thị bảng chọn nhân vật
- [ ] Thêm chức năng tạo gió trong game

## Low Priority
- [ ] Xây dựng chức năng PvE (đấu với máy)
- [ ] Trau chuốt UI
- [ ] Thêm OST, BGM cho game
- [ ] Tối ưu hóa con game
