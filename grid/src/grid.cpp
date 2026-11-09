#include "raylib.h"

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 1000, "Grid");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();

    float height = GetScreenHeight();
    float width  = GetScreenWidth();

    for (int x = 0; x <= width; x++) {
      if ((int)(x - width / 2) % 100 == 0) {
        DrawLineDashed({(float)x, 0.0}, {(float)x, height}, 10, 5, GRAY);
      }
    }

    for (int y = 0; y <= height; y++) {
      if ((int)(y - height / 2) % 100 == 0) {
        DrawLineDashed({0.0, (float)y}, {width, (float)y}, 10, 5, GRAY);
      }
    }

    ClearBackground(BLACK);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
