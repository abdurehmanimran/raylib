#include "raylib.h"
#include "raymath.h"

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(800, 1000, "Grid");

  SetTargetFPS(60);

  float x_offset = 0;
  float y_offset = 0;

  while (!WindowShouldClose()) {
    BeginDrawing();

    float height = GetScreenHeight();
    float width  = GetScreenWidth();

    // Arrow Key Movements
    if (IsKeyDown(KEY_RIGHT)) {
      x_offset += 5;
    }
    if (IsKeyDown(KEY_LEFT)) {
      x_offset -= 5;
    }

    if (IsKeyDown(KEY_UP)) {
      y_offset -= 5;
    }
    if (IsKeyDown(KEY_DOWN)) {
      y_offset += 5;
    }

    // Touchpad Movements
    if (GetMouseWheelMoveV().x > 0) {
      x_offset = Lerp(x_offset, x_offset - 50.0, GetFrameTime() * 10.0);
    }
    if (GetMouseWheelMoveV().x < 0)
      x_offset = Lerp(x_offset, x_offset + 50.0, GetFrameTime() * 10.0);

    if (GetMouseWheelMoveV().y > 0) {
      y_offset = Lerp(y_offset, y_offset - 50.0, GetFrameTime() * 10.0);
    }
    if (GetMouseWheelMoveV().y < 0) {
      y_offset = Lerp(y_offset, y_offset + 50.0, GetFrameTime() * 10.0);
    }

    for (int x = 0; x <= width; x++) {
      if ((int)(x + x_offset - width / 2) % 100 == 0) {
        DrawLineDashed({(float)x, 0.0}, {(float)x, height}, 10, 5, GRAY);
      }
    }

    for (int y = 0; y <= height; y++) {
      if ((int)(y + y_offset - height / 2) % 100 == 0) {
        DrawLineDashed({0.0, (float)y}, {width, (float)y}, 10, 5, GRAY);
      }
    }

    ClearBackground(BLACK);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
