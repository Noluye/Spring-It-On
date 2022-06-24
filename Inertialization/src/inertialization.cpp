#include "common.h"

//--------------------------------------

void inertialize_transition(
	float& off_x, float& off_v,
	float src_x, float src_v,
	float dst_x, float dst_v)
{
	off_x = (src_x + off_x) - dst_x;
	off_v = (src_v + off_v) - dst_v;
}

void inertialize_update(
	float& out_x, float& out_v,
	float& off_x, float& off_v,
	float in_x, float in_v,
	float halflife,
	float dt)
{
	decay_spring_damper_implicit(off_x, off_v, halflife, dt);
	out_x = in_x + off_x;
	out_v = in_v + off_v;
}

void inertialize_sin_func(float& g, float& gv, float t, float freq, float amp, float phase, float off)
{
	g = amp * sin(t * freq + phase) + off;
	gv = amp * freq * cos(t * freq + phase);
}

void inertialize_sin_func_low_freq(float& g, float& gv, float t)
{
	// low frequency
	inertialize_sin_func(g, gv, t, 2.0f * PI * 1.25, 74.0, 23.213123, 254);
}

void inertialize_sin_func_high_freq(float& g, float& gv, float t)
{
	// high frequency
	inertialize_sin_func(g, gv, t, 2.0f * PI * 3.4, 28.0, 912.2381, 113);
}

//--------------------------------------

enum
{
	HISTORY_MAX = 256
};

struct DummyData
{
	float x;  // value
	float v;  // velocity
	float t;  // for draw and calculate velocity
	float g;  // goal
};

class DataWrapper
{
public:
	DataWrapper() = default;
	DataWrapper(float x, float v, float t, float g)
	{
		for (int i = 0; i < HISTORY_MAX; i++) data[i] = DummyData{ x, v, t, g };
	}
	void ShiftHistory()
	{
		for (int i = HISTORY_MAX - 1; i > 0; i--) data[i] = data[i - 1];
	}
	DummyData& operator[](int i) {
		return data[i];
	}
	inline Vector2 produce_point(float x, float y)
	{
		return { x_offset - (data[0].t - x) * timescale, y };
	}
	void Draw()
	{
		DrawCircleV(Vector2{ x_offset, data[0].g}, 5, MAROON);
		DrawCircleV(Vector2{ x_offset, data[0].x }, 5, DARKBLUE);

		for (int i = 0; i < HISTORY_MAX - 1; i++)
		{
			Vector2 g_start = produce_point(data[i].t, data[i].g);
			Vector2 g_stop = produce_point(data[i+1].t, data[i+1].g);
			
			DrawLineV(g_start, g_stop, MAROON);
			DrawCircleV(g_start, 2, MAROON);

			Vector2 x_start = produce_point(data[i].t, data[i].x);
			Vector2 x_stop = produce_point(data[i + 1].t, data[i + 1].x);

			DrawLineV(x_start, x_stop, BLUE);
			DrawCircleV(x_start, 2, BLUE);
		}
	}
private:
	DummyData data[HISTORY_MAX];
	const float timescale = 240.0f;
	const float x_offset = 600.0f;
};


int main(void)
{
	// Init Window
	const int screenWidth = 640;
	const int screenHeight = 360;
	InitWindow(screenWidth, screenHeight, "raylib [springs] example - inertialization");
	// -----------------------------------------------------
	// Init Variables
	float x = screenHeight / 2.0f;
	float v = 0.0;
	float t = 0.0;
	float g = screenHeight / 2.0f;
	float halflife = 0.1f;
	float dt = 1.0 / 60.0f;
	float off_x = 0.0;
	float off_v = 0.0;
	bool func_toggle = false;
	DataWrapper dummy(x, v, t, g);
	// -----------------------------------------------------
	SetTargetFPS(1.0f / dt);
	while (!WindowShouldClose())
	{
		dummy.ShiftHistory();

		if (GuiButton(Rectangle{ 100, 75, 120, 20 }, "Transition"))
		{
			func_toggle = !func_toggle;

			float dst_x, dst_v;
			// given time, get dst_x and dst_v
			if (func_toggle) inertialize_sin_func_low_freq(dst_x, dst_v, t);
			else inertialize_sin_func_high_freq(dst_x, dst_v, t);

			// now 1 means current, before dst
			float src_x = dummy[1].g;
			float src_v = (dummy[1].g - dummy[2].g) / (dummy[1].t - dummy[2].t);

			inertialize_transition(
				off_x, off_v,
				src_x, src_v,
				dst_x, dst_v);
		}

		halflife = GuiSliderBar(Rectangle{ 100, 20, 120, 20 }, "halflife", TextFormat("%5.3f", halflife), halflife, 0.0f, 1.0f);
		dt = GuiSliderBar(Rectangle{ 100, 45, 120, 20 }, "dt", TextFormat("%5.3f", dt), dt, 1.0 / 60.0f, 0.1f);
		t += dt;
		SetTargetFPS(1.0f / dt);
		
		float cur_gv;
		if (func_toggle) inertialize_sin_func_low_freq(g, cur_gv, t);
		else inertialize_sin_func_high_freq(g, cur_gv, t);
		// Update Spring
		inertialize_update(x, v, off_x, off_v, g, cur_gv, halflife, dt);
		
		dummy[0] = { x, v, t, g };

		BeginDrawing();
		ClearBackground(RAYWHITE);
		dummy.Draw();
		EndDrawing();

	}

	CloseWindow();

	return 0;
}