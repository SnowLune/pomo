#include <iostream>
#include <chrono>
#include <thread>

#include <raylib.h>

const int FPS_TARGET		= 60;

const float WORK_MINUTES 	= 25;
const float SHORT_BREAK_MINUTES	= 5;
const float LONG_BREAK_MINUTES	= 30;

// Durations for Work, Short Break, Long Break
const int interval_s[3] = {
	(int)(WORK_MINUTES * 60),
	(int)(SHORT_BREAK_MINUTES * 60),
	(int)(LONG_BREAK_MINUTES * 60)
};

const char* phase_names[3] = {
	"WORK",
	"SHORT BREAK",
	"LONG BREAK"
};

enum phase
{
	work, short_break, long_break
};

struct timer
{
	Sound	tick_sound;
	Sound	ring_sound;
	float	current_time_s;
	int	total_time_s;
	int	work_count;
	phase	phase;
	bool	running;
};

timer
create_timer (void)
{
	// Create
	timer t;

	// Initialize
	t.phase = work;
	t.total_time_s = interval_s[t.phase];
	t.current_time_s = 0.0;
	t.running = false;
	t.work_count = 0;

	return t;
}

void
start_timer (timer* t)
{
	t->running = true;

	while (t->current_time_s < t->total_time_s)
	{
		const short sec_ms = 1000;
		std::this_thread::sleep_for(std::chrono::milliseconds(sec_ms));
		
		++(t->current_time_s);

		std::cout << t->current_time_s
			  << "/"
			  << t->total_time_s
			  << std::endl;
	}

	if (t->current_time_s >= t->total_time_s)
	{
		t->running = false;
	}
}

void
update_timer (timer* t, float ds)
{
	t->current_time_s += ds;
	
	if (t->current_time_s >= t->total_time_s)
	{
		if (t->phase == work)
		{
			if (t->work_count && t->work_count % 4 == 0)
			{
				t->phase = long_break;
			}
			else
			{
				t->phase = short_break;
			}
			
			++t->work_count;
		}
		else
			t->phase = work;

		t->total_time_s = interval_s[t->phase];
		t->current_time_s = 0.0;

		// Play ring sound on interval completion
		if (!IsSoundPlaying(t->ring_sound)) PlaySound(t->ring_sound);
	}
	else
	{
		if (!IsSoundPlaying(t->tick_sound)) PlaySound(t->tick_sound);
	}
}

void
stop_timer (timer* t)
{
	t->running = false;
}

void
set_timer (timer* t, phase phase, float time)
{
	t->phase = phase;
	t->total_time_s = interval_s[t->phase];
	t->current_time_s = time;
}

int
main (void)
{
	const int screen_width = 640;
	const int screen_height = 480;

	const char* SOUND_TICK_FILE = "./sounds/timertick.ogg";
	const char* SOUND_RING_FILE = "./sounds/timerring.ogg";

	const char* FONT_FILE = "./fonts/Mx437_IBM_VGA_9x14.ttf";

	timer pomodoro = create_timer();
	timer* t = &pomodoro;
	t->running = true;

	InitWindow(screen_width, screen_height, "pomo");
	SetTargetFPS(FPS_TARGET);

	Font mono_font = LoadFont(FONT_FILE);

	InitAudioDevice();
	t->tick_sound = LoadSound(SOUND_TICK_FILE);
	t->ring_sound = LoadSound(SOUND_RING_FILE);

	while (!WindowShouldClose())
	{
		const char* status_text = TextFormat(
			"%s - %.3f/%d",
			phase_names[t->phase],
			t->current_time_s,
			t->total_time_s
		);

		BeginDrawing();

		ClearBackground(RAYWHITE);

		Vector2 text_pos = {10, 10};
		DrawTextEx(
			mono_font,
			status_text,
			text_pos,
			14.25,
			0.0,
			BLACK
		);

		EndDrawing();

		if (t->running)
			update_timer(t, GetFrameTime());
	}

	CloseWindow();

	return 0;
}
