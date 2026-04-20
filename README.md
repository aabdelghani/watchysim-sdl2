# watchysim-sdl2

A Linux / SDL2 port of [LeeHolmes/watchysim](https://github.com/LeeHolmes/watchysim) — a simulator for [Watchy](https://watchy.sqfmi.com/) e-paper watch faces. The upstream project is Windows-only (Win32 + GDI+); this fork replaces the platform layer with SDL2 so watch faces can be developed on Linux without the physical hardware.

![watchysim-sdl2 running the 7_SEG watch face](docs/screenshot.png)

## What's different from upstream

| | Upstream | This port |
|---|---|---|
| Platform | Windows (Visual Studio) | Linux (SDL2) |
| Display backend | GDI+ `Graphics`/`HDC` | 200×200 RGB framebuffer → `SDL_Texture` |
| Main loop | `wWinMain` + `WndProc` | `SDL_PollEvent` |
| Build | `WatchySim.sln` | `CMakeLists.txt` |
| Menu-driven state overrides | Yes (battery/weather/steps…) | Dropped — faces see defaults |

Watch face code, fonts, icons, and Bresenham drawing primitives are unchanged.

## Requirements

- CMake ≥ 3.16
- A C++17 compiler (gcc/clang)
- SDL2 development headers

On Debian/Ubuntu:

```
sudo apt install cmake build-essential libsdl2-dev
```

## Build & run

```
cmake -B build
cmake --build build -j
./build/watchysim
```

`ESC` or `Q` quits. `R` forces a redraw.

The sim shows a 200×200 area upscaled 4×. Time advances with the system clock (minute resolution).

## Switching watch faces

Edit `WatchySim/main_sdl.cpp` — change the include and the instantiated class:

```cpp
#include "WatchFaces/PowerShell/Watchy_PowerShell.h"
...
WatchyPowerShell watchy;
```

Available faces under `WatchySim/WatchFaces/`: `7_SEG`, `AnalogGabel`, `DOS`, `DrawTest`, `MacPaint`, `Mario`, `Niobe`, `Pokemon`, `PowerShell`, `Scene`, `Tetris`.

## Writing a new watch face

Same workflow as upstream — see the original [README's "Testing a Watch Face"](https://github.com/LeeHolmes/watchysim#testing-a-watch-face) section. Add a class extending `Watchy`, override `drawWatchFace()`, drop it in `WatchFaces/MyFace/`. CMake's glob picks up any `WatchFaces/*/*.cpp` on the next build.

## Porting notes

The main boundary crossed was `DisplaySim` (in `WatchySim/Watchy.h` / `Watchy.cpp`), which previously held `Gdiplus::Graphics*` and `HDC*` pointers. The port replaces these with an owned `uint8_t pixels[200*200*3]` RGB24 framebuffer. `drawPixel`/`drawLine`/`fillScreen` now write to the buffer directly (Bresenham for lines), and `main_sdl.cpp` streams that buffer into an `SDL_Texture` once per minute. A small `compat.h` shims MSVC-only `_itoa`/`_itoa_s`/`_ltoa`/`_ultoa` so `arduino/String.cpp` and a few watch faces compile unchanged.

## License

MIT, inherited from the upstream project. See [LICENSE](LICENSE).
