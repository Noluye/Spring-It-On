#include "common.h"

enum
{
	HISTORY_MAX = 256
};

float x_prev[HISTORY_MAX];
float t_prev[HISTORY_MAX];

enum class DamperType
{
	SIMPLE,
	BAD,
	EXPONENTIAL,
	IMPLICIT
};

int main(void)
{
	// Init Window

	const int screenWidth = 640;
	const int screenHeight = 360;

	InitWindow(screenWidth, screenHeight, "raylib [springs] example - damper");

	// Init Variables
	DamperType damper_type = DamperType::IMPLICIT;
	float t = 0.0;
	float x = screenHeight / 2.0f;
	float g = x;
	float goal_offset = 600;
	float factor = 0.1;

	float damping = 10.0f;
	float halflife = 0.1f;
	float dt = 1.0 / 60.0f;
	float timescale = 240.0f;

	SetTargetFPS(1.0f / dt);

	for (int i = 0; i < HISTORY_MAX; i++)
	{
		x_prev[i] = x;
		t_prev[i] = t;
	}

	while (!WindowShouldClose())
	{
		// Shift History

		for (int i = HISTORY_MAX - 1; i > 0; i--)
		{
			x_prev[i] = x_prev[i - 1];
			t_prev[i] = t_prev[i - 1];
		}

		// Get Goal
		if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) g = GetMousePosition().y;

		dt = GuiSliderBar(Rectangle{ 100, 45, 120, 20 }, "dt", TextFormat("%5.3f", dt), dt, 1.0 / 60.0f, 0.1f);
		SetTargetFPS(1.0f / dt);
		t += dt;

		// Update Spring
		switch (damper_type)
		{
		case DamperType::SIMPLE:
			factor = GuiSliderBar(Rectangle { 100, 20, 120, 20 }, "factor", TextFormat("%5.3f", factor), factor, 0.0f, 1.0f);
			x = damper(x, g, factor);
			break;
		case DamperType::BAD:
			damping = GuiSliderBar(Rectangle { 100, 20, 120, 20 }, "damping", TextFormat("%5.3f", damping), damping, 0.01f, 30.0f);
			x = damper_bad(x, g, damping, dt);
			break;
		case DamperType::EXPONENTIAL:
			damping = GuiSliderBar(Rectangle{ 100, 20, 120, 20 }, "damping", TextFormat("%5.3f", damping), damping, 0.01f, 30.0f);
			x = damper_exponential(x, g, damping, dt);
			break;
		case DamperType::IMPLICIT:
			halflife = GuiSliderBar(Rectangle{ 100, 20, 120, 20 }, "halflife", TextFormat("%5.3f", halflife), halflife, 0.0f, 1.0f);
			x = damper_implicit(x, g, halflife, dt);
			break;
		default:
			break;
		}

		x_prev[0] = x;
		t_prev[0] = t;

		BeginDrawing();

		ClearBackground(RAYWHITE);

		DrawCircleV(Vector2{ goal_offset, g }, 5, MAROON);
		DrawCircleV(Vector2{ goal_offset, x }, 5, DARKBLUE);

		for (int i = 0; i < HISTORY_MAX - 1; i++)
		{
			Vector2 x_start = { goal_offset - (t - t_prev[i + 0]) * timescale, x_prev[i + 0] };
			Vector2 x_stop = { goal_offset - (t - t_prev[i + 1]) * timescale, x_prev[i + 1] };

			DrawLineV(x_start, x_stop, BLUE);
			DrawCircleV(x_start, 2, BLUE);
		}


		EndDrawing();

	}

	CloseWindow();

	return 0;
}