#include "raylib.h"
#include "raymath.h"

void drawGridLines(int x_start, int y_start, int width, int height, Color clr) {
  for (int x = x_start - width; x <= x_start + 2 * width; x++) {
    if ((int)(x) % 100 == 0) {
      DrawLineDashed({(float)x, (float)y_start - height},
                     {(float)x, (float)y_start + 2 * height}, 10, 5, clr);
    }
  }

  for (int y = y_start - height; y <= y_start + 2 * height; y++) {
    if ((int)(y) % 100 == 0) {
      DrawLineDashed({(float)x_start - width, (float)y},
                     {(float)x_start + 2 * width, (float)y}, 10, 5, clr);
    }
  }
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_MSAA_4X_HINT);

  float height = 1000;
  float width  = 1200;

  InitWindow(1200, 1000, "Grid");

  SetTargetFPS(60);

  float x_offset = 0;
  float y_offset = 0;

  Camera2D map{};
  map.target   = (Vector2){0, 0};
  map.offset   = (Vector2){width / 2, height / 2};
  map.rotation = 0.0f;
  map.zoom     = 1.0f;

  while (!WindowShouldClose()) {
    height = GetScreenHeight();
    width  = GetScreenWidth();

    if (IsKeyDown(KEY_RIGHT))
      map.target.x += 5;
    if (IsKeyDown(KEY_LEFT))
      map.target.x -= 5;

    if (IsKeyDown(KEY_UP))
      map.target.y -= 5;
    if (IsKeyDown(KEY_DOWN))
      map.target.y += 5;

    if (IsKeyDown(KEY_Q))
      map.rotation--;
    if (IsKeyDown(KEY_E))
      map.rotation++;

    if (map.rotation > 40)
      map.rotation = 40;
    else if (map.rotation < -40)
      map.rotation = -40;

    if (IsKeyDown(KEY_LEFT_CONTROL))
      map.zoom = expf(logf(map.zoom) + ((float)GetMouseWheelMove() * 0.1f));

    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode2D(map);

    int x_start = GetScreenToWorld2D({0, 0}, map).x;
    int y_start = GetScreenToWorld2D({0, 0}, map).y;

    drawGridLines(x_start, y_start, width, height, DARKGRAY);

    EndMode2D();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
