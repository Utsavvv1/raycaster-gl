# Raycaster (OpenGL)

**Classic raycasting engine in modern C++ — CPU-side scene generation, OpenGL for display.**

Use the sections below as copy-paste material for a resume or portfolio site; trim bullets to fit one line each if needed.

---

## At a glance (resume / CV)

- **Graphics:** OpenGL (GLAD), custom GLSL shaders, fullscreen textured quad; nearest-neighbor sampling for crisp software-rendered output.
- **Engine:** C++20, CMake 3.22+, modular split between core rendering (window, mesh, shaders, raster API) and game code (tile map, camera/player state).
- **Techniques:** Arbitrary-resolution CPU framebuffer (`RGB8` texture upload per frame); 2D grid level with minimap; player position and facing with direction vector for HUD-style overlays.
- **Tooling:** Conan for dependencies (`fmt`, `glfw3`); `compile_commands` export for IDE/static analysis compatibility.

---

## Tech stack

| Area | Choices |
|------|---------|
| Language | C++20 |
| Build | CMake, Conan (`fmt`, GLFW) |
| Graphics | OpenGL, GLAD, GLFW |
| Shaders | Vertex/fragment pipeline, 2D texture sampling |

---

## What this project demonstrates

1. **Hybrid rendering path** — Scene work can be done on the CPU (ray casting, sprites, effects) while OpenGL only presents the result: one quad, one texture, predictable cost.
2. **Small engine layers** — `Pixels` exposes simple 2D primitives (lines, rects, fill/stroke) backed by a byte buffer; the game updates that buffer each frame.
3. **Data-driven level** — Maze represented as a 2D tile grid; easy to extend with wall IDs, doors, or entity layers.
4. **Graphics fundamentals** — Projection and ray math for a Wolfenstein-style engine are a natural fit; the codebase is structured to add per-column ray tests and wall sampling without fighting the renderer.

**Design note:** The approach is informed by classic Wolfenstein-style math (e.g. polar angles for position/direction) rather than a step-by-step Cartesian tutorial flow. That maps well to casting rays per screen column and choosing wall strips and texture coordinates.

---

## Current behavior

- Loads a tiled level and draws a **minimap** (walls vs. floor).
- Draws the **player** on the minimap with a **facing indicator** (direction line).
- Runs a ** GLFW window loop** with timer-driven updates and a display path from buffer → texture → screen.

---

## Building

Prerequisites: CMake, Conan 2.x, a C++20 compiler, and OpenGL-capable drivers (GLFW resolves most platform GL deps via Conan).

From the project root, a typical Conan + CMake flow:

```bash
mkdir build && cd build
conan install .. --output-folder=. --build=missing -s build_type=Release
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

The executable will be under `build/` (or `build/Release` / `build/Debug`, depending on the generator).

If your Conan install writes the toolchain beside the source folder instead, pass the path Conan prints after `conan install` (`-DCMAKE_TOOLCHAIN_FILE=...`).

---

## Repository layout (high level)

- `src/core/` — Window, shaders, mesh, software raster surface, timing.
- `src/game/` — Level grid, player, game loop wiring.
- `src/shaders/` — GLSL for the presentation pass.

---

## Possible next steps (portfolio evolution)

- Implement per-column ray casting in the framebuffer and texture-mapped vertical strips.
- Input: strafe / rotate, collision against the tile grid.
- Optional: billboards or props using the same buffer.

---

## License

Add a `LICENSE` file in this repo if you publish it; ensure compliance with any third-party code (e.g. GLAD, dependency licenses).
