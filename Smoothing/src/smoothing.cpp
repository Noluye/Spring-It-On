#include "common.h"

enum
{
	HISTORY_MAX = 256
};

struct DummyData
{
	float x;
	float v;
	float t;
};


class DummyDataWrapper
{
public:
	DummyDataWrapper() = default;
	DummyDataWrapper(float x, float v, float t) { Init(x, v, t); }
	void Init(float x, float v, float t)
	{
		for (int i = 0; i < HISTORY_MAX; i++) data[i] = DummyData(x, v, t);
	}
	void ShiftHistory()
	{
		for (int i = HISTORY_MAX - 1; i > 0; i--) data[i] = data[i - 1];
	}
	DummyData& operator[](int i) { return data[i]; }
	void Draw(float g) const
	{
		DrawCircleV(Vector2{ goal_offset, g }, 5, MAROON);
		DrawCircleV(Vector2{ goal_offset, data[0].x }, 5, DARKBLUE);

		for (int i = 0; i < HISTORY_MAX - 1; i++)
		{
			Vector2 x_start = { goal_offset - (data[0].t - data[i + 0].t) * timescale, data[i + 0].x };
			Vector2 x_stop = { goal_offset - (data[0].t - data[i + 1].t) * timescale, data[i + 1].x };

			DrawLineV(x_start, x_stop, BLUE);
			DrawCircleV(x_start, 2, BLUE);
		}
	}
private:
	DummyData data[HISTORY_MAX];
	const float goal_offset = 600;
	const float timescale = 240.0f;
};

int main(void)
{
	// Init Window
	const int screenWidth = 640;
	const int screenHeight = 360;
	InitWindow(screenWidth, screenHeight, "raylib [springs] example - smoothing");

	// Init Variables
	float t = 0.0;
	float x = screenHeight / 2.0f;
	float v = 0.0;
	float g = x;

	float halflife = 0.1f;
	float dt = 1.0 / 60.0f;

	float noise = 0.0f;
	float jitter = 0.0f;

	SetTargetFPS(1.0f / dt);

	DummyDataWrapper dummy(x, v, t);

	while (!WindowShouldClose())
	{
		// Shift History
		dummy.ShiftHistory();

		// Get Goal
		if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) g = GetMousePosition().y;

		g += noise * (((float)rand() / RAND_MAX) * 2.0f - 1.0);

		if (jitter)
		{
			g -= jitter;
			jitter = 0.0;
		}
		else if (rand() % (int)(0.5 / dt) == 0)
		{
			jitter = noise * 10.0f * (((float)rand() / RAND_MAX) * 2.0f - 1.0);
			g += jitter;
		}

		// Spring Damper

		halflife = GuiSliderBar(Rectangle{ 100, 20, 120, 20 }, "halflife", TextFormat("%5.3f", halflife), halflife, 0.0f, 1.0f);
		dt = GuiSliderBar(Rectangle{ 100, 45, 120, 20 }, "dt", TextFormat("%5.3f", dt), dt, 1.0 / 60.0f, 0.1f);
		noise = GuiSliderBar(Rectangle{ 100, 70, 120, 20 }, "noise", TextFormat("%5.3f", noise), noise, 0.0f, 20.0f);

		// Update Spring

		SetTargetFPS(1.0f / dt);

		t += dt;

		simple_spring_damper_implicit(x, v, g, halflife, dt);

		dummy[0] = { x, v, t };

		BeginDrawing();

		ClearBackground(RAYWHITE);
		dummy.Draw(g);
		EndDrawing();

	}

	CloseWindow();

	return 0;
}