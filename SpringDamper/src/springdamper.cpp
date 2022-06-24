#include "common.h"

enum
{
	HISTORY_MAX = 256
};

enum class SpringDamperType
{
	BAD = 0,
	IMPLICIT1,
	IMPLICIT2,            // with oscillation(IMPORTANT)
	CRITICAL_IMPLICIT,    // without oscillation and easy to compute(IMPORTANT)
	SIMPLE_IMPLICIT,      // with no special cases for over-damping and under-damping
	DECAY_IMPLICIT,       // when the goal position is zero (constant g)
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
			Vector2 x_stop =  { goal_offset - (data[0].t - data[i + 1].t) * timescale, data[i + 1].x };

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

	InitWindow(screenWidth, screenHeight, "raylib [springs] example - springdamper");

	// Init Variables
	float t = 0.0;
	float x = screenHeight / 2.0f;
	float v = 0.0;
	float g = x;

	float stiffness = 15.0f;
	float damping = 2.0f;

	float frequency = 2.0f;
	float halflife = 0.1f;
	float dt = 1.0 / 60.0f;
	SetTargetFPS(1.0f / dt);

	DummyDataWrapper dummy = DummyDataWrapper(x, v, t);
	int dropdown_active_index = 5;
	SpringDamperType spring_dampler_type = SpringDamperType::BAD;
	bool dropdown_mode = false;


	while (!WindowShouldClose())
	{
		dummy.ShiftHistory();

		// Get Goal
		if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) g = GetMousePosition().y;
		// Choose damper type
		if (GuiDropdownBox(Rectangle { 300, 20, 120, 20 },
			"BAD;IMPLICIT1;IMPLICIT2;CRITICAL_IMPLICIT;SIMPLE_IMPLICIT;ECAY_IMPLICIT", 
			& dropdown_active_index, dropdown_mode)) dropdown_mode = !dropdown_mode;
		spring_dampler_type = (SpringDamperType)dropdown_active_index;

		dt = GuiSliderBar(Rectangle{ 100, 70, 120, 20 }, "dt", TextFormat("%5.3f", dt), dt, 1.0 / 60.0f, 0.1f);
		SetTargetFPS(1.0f / dt);
		t += dt;

		// Update Spring
		switch (spring_dampler_type)
		{
		case SpringDamperType::BAD:
			stiffness = GuiSliderBar(Rectangle{ 100, 20, 120, 20 }, "stiffness", TextFormat("%5.3f", stiffness), stiffness, 0.01f, 30.0f);
			damping = GuiSliderBar(Rectangle{ 100, 45, 120, 20 }, "damping", TextFormat("%5.3f", damping), damping, 0.01f, 30.0f);
			spring_damper_bad(x, v, g, 0.0f, stiffness, damping, dt);
			break;
		case SpringDamperType::IMPLICIT1:
			stiffness = GuiSliderBar(Rectangle{ 100, 20, 120, 20 }, "stiffness", TextFormat("%5.3f", stiffness), stiffness, 0.01f, 30.0f);
			damping = GuiSliderBar(Rectangle{ 100, 45, 120, 20 }, "damping", TextFormat("%5.3f", damping), damping, 0.01f, 30.0f);
			spring_damper_implicit0(x, v, g, 0.0f, stiffness, damping, dt);
			break;
		case SpringDamperType::IMPLICIT2:
			frequency = GuiSliderBar(Rectangle{ 100, 20, 120, 20 }, "frequency", TextFormat("%5.3f", frequency), frequency, 0.0f, 3.0f);
			halflife = GuiSliderBar(Rectangle{ 100, 45, 120, 20 }, "halflife", TextFormat("%5.3f", halflife), halflife, 0.0f, 1.0f);
			spring_damper_implicit(x, v, g, 0.0f, frequency, halflife, dt);
			break;
		case SpringDamperType::CRITICAL_IMPLICIT:
			halflife = GuiSliderBar(Rectangle{ 100, 20, 120, 20 }, "halflife", TextFormat("%5.3f", halflife), halflife, 0.0f, 1.0f);
			critical_spring_damper_implicit(x, v, g, 0.0f, halflife, dt);
			break;
		case SpringDamperType::SIMPLE_IMPLICIT:
			halflife = GuiSliderBar(Rectangle{ 100, 20, 120, 20 }, "halflife", TextFormat("%5.3f", halflife), halflife, 0.0f, 1.0f);
			simple_spring_damper_implicit(x, v, g, halflife, dt);
			break;
		case SpringDamperType::DECAY_IMPLICIT:
			halflife = GuiSliderBar(Rectangle{ 100, 20, 120, 20 }, "halflife", TextFormat("%5.3f", halflife), halflife, 0.0f, 1.0f);
			decay_spring_damper_implicit(x, v, halflife, dt);
			break;
		default:
			break;
		}

		dummy[0] = { x, v, t };

		BeginDrawing();

		ClearBackground(RAYWHITE);

		dummy.Draw(g);

		EndDrawing();

	}

	CloseWindow();

	return 0;
}