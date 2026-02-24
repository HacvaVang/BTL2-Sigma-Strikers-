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
