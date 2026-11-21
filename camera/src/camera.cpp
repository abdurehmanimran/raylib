#include "raylib.h"
#include "raymath.h"

class Map {
public:
  Map(Vector2 offset) : camera{offset, (Vector2){0, 0}, 0.0f, 1.0f} {}

  int getX(Vector2 point) const { return GetScreenToWorld2D(point, camera).x; }
  int getY(Vector2 point) const { return GetScreenToWorld2D(point, camera).y; }

  Vector2 getMapPoint(Vector2 point) const {
    return GetScreenToWorld2D(point, camera);
  }

  void startDraw() const {
    Camera2D c = camera;
    c.target.x = roundf(c.target.x * c.zoom) / c.zoom;
    c.target.y = roundf(c.target.y * c.zoom) / c.zoom;
    BeginMode2D(c);
  }

  void reCenter() {
    camera.offset = {(float)(GetScreenWidth() / 2.0f),
                     (float)(GetScreenHeight() / 2.0f)};
  }

  void endDraw() const { EndMode2D(); }

  void showGrid(int spacing = 100) const {
    const float w = (float)GetScreenWidth();
    const float h = (float)GetScreenHeight();

    // NOTE: We cannot draw grid lines for all of the map as it is infinite

    // Points of world map that are appearing on the corners of our screen
    //(a rotated rectangle when camera.rotation != 0)
    Vector2 corners[4] = {
    getMapPoint({0, 0}),
    getMapPoint({w, 0}),
    getMapPoint({w, h}),
    getMapPoint({0, h}),
    };

    // Axis-aligned bounding box of that rotated rectangle
    float minX = corners[0].x, maxX = corners[0].x;
    float minY = corners[0].y, maxY = corners[0].y;
    for (int i = 1; i < 4; i++) {
      minX = fminf(minX, corners[i].x);
      maxX = fmaxf(maxX, corners[i].x);
      minY = fminf(minY, corners[i].y);
      maxY = fmaxf(maxY, corners[i].y);
    }

    // Snap outward to multiples of spacing
    const float left   = floorf(minX / spacing) * spacing;
    const float right  = ceilf(maxX / spacing) * spacing;
    const float top    = floorf(minY / spacing) * spacing;
    const float bottom = ceilf(maxY / spacing) * spacing;

    for (float x = left; x <= right; x += spacing)
      DrawLineDashed({x, top}, {x, bottom}, 10, 5, gridClr);

    for (float y = top; y <= bottom; y += spacing)
      DrawLineDashed({left, y}, {right, y}, 10, 5, gridClr);
  }

  // Moves the view so the grid shifts by `screenDelta` pixels on screen,
  // correctly accounting for camera rotation and zoom.

  Vector2 panDelta(Vector2 screenDelta) const {
    Vector2 world = Vector2Rotate(Vector2Scale(screenDelta, 1.0f / camera.zoom),
                                  -camera.rotation * DEG2RAD);
    return Vector2Negate(world);
  }

  void handleMouse() {
    const float dt = GetFrameTime();
    const bool  dragNow =
    IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    if (dragNow) {
      if (!dragging)
        velocity = {0, 0}; // a new grab stops any glide
      dragging = true;

      // 1:1 tracking: move the camera itself, and keep the goal in sync
      Vector2 move  = panDelta(GetMouseDelta());
      camera.target = Vector2Add(camera.target, move);
      goal          = camera.target;

      // Smoothed velocity estimate (world units / second) for the release glide
      if (dt > 0.0f)
        velocity = Vector2Lerp(velocity, Vector2Scale(move, 1.0f / dt), 0.5f);
    } else {
      dragging = false;

      // Inertia: keep gliding after release, decaying exponentially
      goal     = Vector2Add(goal, Vector2Scale(velocity, dt));
      velocity = Vector2Scale(velocity, expf(-friction * dt));
      if (Vector2Length(velocity) < 5.0f)
        velocity = {0, 0};
    }

    // Scroll
    Vector2 wheel = GetMouseWheelMoveV();
    if (IsKeyDown(KEY_LEFT_SHIFT) && wheel.x == 0.0f) {
      wheel.x = -wheel.y;
      wheel.y = 0.0f;
    }

    if (wheel.x != 0.0f || wheel.y != 0.0f) {
      Vector2 step = {wheel.x * scrollPower, wheel.y * scrollPower};
      goal         = Vector2Add(goal, panDelta(step));
    }
  }

  void handleKeyboard() {
    const float dt = GetFrameTime();

    // Arrow keys: direction the *view* should move, in screen space
    Vector2 dir{0.0f, 0.0f};
    if (IsKeyDown(KEY_RIGHT))
      dir.x += 1.0f;
    if (IsKeyDown(KEY_LEFT))
      dir.x -= 1.0f;
    if (IsKeyDown(KEY_DOWN))
      dir.y += 1.0f;
    if (IsKeyDown(KEY_UP))
      dir.y -= 1.0f;

    if (dir.x != 0.0f || dir.y != 0.0f) {
      dir = Vector2Normalize(dir);
      // panDelta gives the *grid* shift, so subtract it to move the view
      // instead
      goal = Vector2Subtract(goal, panDelta(Vector2Scale(dir, keySpeed * dt)));
    }

    // Q / E: rotate the view
    if (IsKeyDown(KEY_E))
      goalRotation += rotationSpeed * dt;
    if (IsKeyDown(KEY_Q))
      goalRotation -= rotationSpeed * dt;
  }

  void handleInput() {
    handleKeyboard();
    handleMouse();
  }

  // Smoothly move our camera target to the destination goal
  void update() {
    const float t   = 1.0f - expf(-smoothing * GetFrameTime());
    camera.target   = Vector2Lerp(camera.target, goal, t);
    camera.rotation = Lerp(camera.rotation, goalRotation, t);
  }

private:
  Camera2D camera{};
  Color    gridClr{GRAY};
  int      movementPower{20};

  Vector2 goal{0, 0};
  Vector2 velocity{0, 0};
  bool    dragging{false};

  float keySpeed{400.0f}; // arrow-key speed

  float scrollPower{60.0f}; // pixels per wheel notch
  float smoothing{18.0f};
  float friction{6.0f};

  float goalRotation{0.0f};
  float rotationSpeed{100.0f};
};

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

  float height = 1000;
  float width  = 1200;

  InitWindow(1200, 1000, "Grid");
  SetTargetFPS(60);

  Map mainMap{{width / 2, height / 2}};

  while (!WindowShouldClose()) {

    mainMap.handleInput();
    mainMap.update();

    BeginDrawing();
    ClearBackground(BLACK);

    mainMap.startDraw();
    mainMap.reCenter(); // Keeps the map at the center of our screen

    DrawCircle(0, 0, 5.0f, GREEN);

    mainMap.showGrid(100);
    mainMap.endDraw();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
