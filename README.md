# Raycaster (OpenGL)

**Classic raycasting engine in modern C++ — CPU-side scene generation, OpenGL for display.**

---

## At a glance 

- **Graphics:** OpenGL (GLAD), custom GLSL shaders, fullscreen textured quad; nearest-neighbor sampling for crisp software-rendered output.
- **Engine:** C++20, CMake 3.22+, modular split between core rendering (window, mesh, shaders, raster API) and game code (tile map, player, raycaster).
- **Techniques:** Arbitrary-resolution CPU framebuffer (`RGB8` texture upload per frame); DDA grid raycasting per screen column; perpendicular distance for fish-eye correction; 2D level with minimap overlay; axis-separated wall collision.
- **Input:** GLFW key polling via `Window` / `Pixels` (`IsKeyDown`); movement and rotation scaled by frame delta for frame-rate independence.
- **Tooling:** Conan 2.x for dependencies (`fmt`, `glfw3`); `compile_commands` export for IDE/static analysis compatibility.

---

## Tech stack

| Area | Choices |
|------|---------|
| Language | C++20 |
| Build | CMake, Conan 2.x (`fmt`, GLFW) |
| Graphics | OpenGL, GLAD, GLFW |
| Shaders | Vertex/fragment pipeline, 2D texture sampling |

---

## What this project demonstrates

1. **Hybrid rendering path** — Scene work on the CPU (ray casting, raster primitives) while OpenGL presents the result: one quad, one texture, predictable cost.
2. **Small engine layers** — `Pixels` exposes 2D primitives (`Line`, `Rect`, `VLine`, `FillRect`, fill/stroke) backed by a byte buffer; the game fills that buffer each frame.
3. **DDA raycasting** — For each horizontal column, a ray steps through the tile grid; wall hit distance drives vertical strip height; X vs Y wall sides use different shades.
4. **Data-driven level** — Maze as a 2D tile grid (`1` = wall, `0` = floor); helpers map world coordinates to tiles for collision and ray tests.
5. **Player and collision** — Position and facing in radians; forward/back and rotate with delta time; small AABB-style clearance against walls so the player cannot walk into blocks.
6. **Graphics fundamentals** — 60° horizontal FOV, camera plane derived from `tan(FOV/2)` for per-column ray directions.

---

## Current behavior

- **3D view** — Ceiling (top half) and floor (bottom half) colors, untextured wall strips in the center.
- **Minimap** — Scaled tile grid and player marker with facing line, drawn in a corner over the 3D view.
- **Controls** — See [Controls](#controls) below.
- **Main loop** — GLFW window, timer-based `delta` in seconds, clear → update → draw → upload buffer → draw fullscreen quad.

### Why it can look smooth and sharp

- **Smooth:** Movement and rotation multiply by `delta` (seconds per frame). `glfwSwapInterval(1)` enables vsync with the display.
- **Sharp:** Texture uses `GL_NEAREST` (no blur); one vertical sample column per screen column; native-resolution CPU buffer.

---

## Controls

| Action | Keys |
|--------|------|
| Move forward | `W` or **Up** |
| Move backward | `S` or **Down** |
| Turn left | `A` or **Left** |
| Turn right | `D` or **Right** |

---

## Building

**Prerequisites:** CMake, **Conan 2.x**, a C++20 compiler, and OpenGL-capable drivers.

### If `conan` is not found (Windows)

Install Conan 2 and ensure it is on `PATH`, for example:

```powershell
py -m pip install --user "conan>=2,<3"
```

Then open a new terminal, or add your Python user `Scripts` folder to `PATH`. First-time setup:

```powershell
conan profile detect --force
```

### Build (all platforms)

From the **project root**:

```bash
mkdir build && cd build
conan install .. --output-folder=. --build=missing -s build_type=Release
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

**Windows (Visual Studio generator):** use an explicit configuration when building:

```powershell
mkdir build
cd build
conan install .. --output-folder=. --build=missing -s build_type=Release
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Run

- **Visual Studio / multi-config:** `build\Release\raycaster-gl.exe` (from the `build` directory).
- **Single-config (e.g. Ninja):** `build\raycaster-gl.exe`.

If Conan writes the toolchain elsewhere, pass the path it prints after `conan install` to `-DCMAKE_TOOLCHAIN_FILE=...`.

---

## Repository layout 

- `src/core/` — Window, shaders, mesh, software raster surface (`Pixels`), timing.
- `src/game/` — Level grid, player (movement, collision, minimap), `main` (raycast loop, input wiring).
- `src/shaders/` — GLSL for the presentation pass.

---

## Possible next steps 

- Texture-mapped walls (per-column `u` coordinate, optional floor/ceiling casting).
- Sprites / billboards in the same framebuffer.
- Strafe keys, mouse look, map loading from files.

