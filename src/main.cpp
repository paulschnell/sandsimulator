#include "defines.hpp"
#include "raylib.h"

#include <cstring>
#include <chrono>
#include <random>

constexpr u16 SCREEN_WIDTH = 800;
constexpr u16 SCREEN_HEIGHT = 600;

constexpr u16 SANDCORN_SIZE = 8;
constexpr u16 CANVAS_WIDTH = SCREEN_WIDTH / SANDCORN_SIZE;
constexpr u16 CANVAS_HEIGHT = SCREEN_HEIGHT / SANDCORN_SIZE;

constexpr u16 BRUSH_RADIUS = 1;
constexpr f32 BRUSH_CHANCE = 0.75;

constexpr f32 UPDATE_TIME = 0.02;

i32 main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sand Simulator");
    SetTargetFPS(60);

    // column-major, because checking y is this way more efficent
    bool* canvas = (bool*) malloc(sizeof(bool) * CANVAS_WIDTH * CANVAS_HEIGHT);
    std::memset(canvas, 0, sizeof(bool) * CANVAS_WIDTH * CANVAS_HEIGHT);

    bool* nextCanvas = (bool*) malloc(sizeof(bool) * CANVAS_WIDTH * CANVAS_HEIGHT);
    std::memset(nextCanvas, 0, sizeof(bool) * CANVAS_WIDTH * CANVAS_HEIGHT);

    f32 timer = UPDATE_TIME;

    std::random_device dev;
    std::mt19937 rng(dev());

    auto prevTime = std::chrono::system_clock::now();
    while (!WindowShouldClose()) {
        auto curTime = std::chrono::system_clock::now();
        std::chrono::duration<f32> diff = curTime - prevTime;
        prevTime = curTime;
        f32 deltaTime = diff.count();

        timer -= deltaTime;
        if (timer <= 0.0f) {
            timer += UPDATE_TIME; // remainder

            // Let it fall
            for (u16 x = 0; x < CANVAS_WIDTH; x++) {
                for (u16 y = 0; y < CANVAS_HEIGHT; y++) {
                    if (canvas[x * CANVAS_HEIGHT + y]) {
                        if (y > 0) {
                            // First Straight Down
                            if (!canvas[x * CANVAS_HEIGHT + y - 1]) {
                                canvas[x * CANVAS_HEIGHT + y] = FALSE;
                                nextCanvas[x * CANVAS_HEIGHT + y - 1] = TRUE;
                                continue;

                                // Then Diagonal Down
                                // left
                            } else if (x > 0 && !canvas[(x - 1) * CANVAS_HEIGHT + y - 1]) {
                                canvas[x * CANVAS_HEIGHT + y] = FALSE;
                                nextCanvas[(x - 1) * CANVAS_HEIGHT + y - 1] = TRUE;
                                continue;
                            } else if (x < CANVAS_WIDTH - 1 && !canvas[(x + 1) * CANVAS_HEIGHT + y - 1]) {
                                canvas[x * CANVAS_HEIGHT + y] = FALSE;
                                nextCanvas[(x + 1) * CANVAS_HEIGHT + y - 1] = TRUE;
                                continue;
                            }
                        }
                        nextCanvas[x * CANVAS_HEIGHT + y] = TRUE;
                    }
                }
            }

            // Swap Canvas
            bool* temp = canvas;
            canvas = nextCanvas;
            nextCanvas = temp;
            std::memset(nextCanvas, FALSE, sizeof(bool) * CANVAS_WIDTH * CANVAS_HEIGHT);

            // Create New Sand
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                u16 xm = GetMouseX() / SANDCORN_SIZE;
                u16 ym = CANVAS_HEIGHT - 1 - GetMouseY() / SANDCORN_SIZE;

                for (i32 x = xm - BRUSH_RADIUS; x <= xm + BRUSH_RADIUS; x++) {
                    for (i32 y = ym - BRUSH_RADIUS; y <= ym + BRUSH_RADIUS; y++) {
                        if (x >= 0 && x < CANVAS_WIDTH && y >= 0 && y < CANVAS_HEIGHT) {
                            std::uniform_real_distribution<f32> dist;
                            if (dist(rng) > BRUSH_CHANCE)
                                canvas[x * CANVAS_HEIGHT + y] = TRUE;
                        }
                    }
                }
            }
        }

        // Delete All Sand
        if (IsKeyPressed(KEY_BACKSPACE)) {
            std::memset(canvas, FALSE, sizeof(bool) * CANVAS_WIDTH * CANVAS_HEIGHT);
        }

        // Rendering
        BeginDrawing();

        ClearBackground(DARKGRAY);

        // Draw Buffer
        for (u16 x = 0; x < CANVAS_WIDTH; x++) {
            for (u16 y = 0; y < CANVAS_HEIGHT; y++) {
                DrawRectangle(x * SANDCORN_SIZE, (CANVAS_HEIGHT - y - 1) * SANDCORN_SIZE, SANDCORN_SIZE, SANDCORN_SIZE, canvas[x * CANVAS_HEIGHT + y] ? WHITE : DARKGRAY);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
