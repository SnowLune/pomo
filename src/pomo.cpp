#include <raylib.h>

#define FPS_TARGET	60

int
main (void)
{
	const int screen_width = 640;
	const int screen_height = 480;

	InitWindow(screen_width, screen_height, "pomo");

	SetTargetFPS(FPS_TARGET);

	while (!WindowShouldClose())
	{
		BeginDrawing();

		ClearBackground(RAYWHITE);
		DrawText("Pomodoro", 10, 10, 20, BLACK);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
