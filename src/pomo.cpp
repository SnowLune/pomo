#include <iostream>
#include <chrono>
#include <thread>

#include <raylib.h>

#define FPS_TARGET		60

#define WORK_MINUTES		25
#define SHORT_BREAK_MINUTES	5
#define LONG_BREAK_MINUTES	30

/*
 * set timer, reset timer, short break, long break
 */

/*
 * so how do i do this ? uhhhh i think i need this to be like
 *  modes for a timer for each phase, and a loop
 *  the loop will be like (work, break [what kind of break]) repeat
 *  yeah that sounds about right
 *  and for like the UI what do i do ? animations, buttons ?
 *
 */

// Durations for Work, Short Break, Long Break
const int interval_s[3] = {
	WORK_MINUTES * 60,
	SHORT_BREAK_MINUTES * 60,
	LONG_BREAK_MINUTES * 60
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
	int	total_time_s;
	float	current_time_s;
	phase	phase;
	bool	running;
	int	work_count;
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
		int sec_ms = 1000;
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

	timer pomodoro = create_timer();
	timer* t = &pomodoro;
	t->running = true;

	InitWindow(screen_width, screen_height, "pomo");
	SetTargetFPS(FPS_TARGET);

	while (!WindowShouldClose())
	{
		const char* status_text = TextFormat(
			"%s - %0.03f/%d",
			phase_names[t->phase],
			t->current_time_s,
			t->total_time_s
		);

		BeginDrawing();

		ClearBackground(RAYWHITE);
		DrawText(status_text, 10, 10, 20, BLACK);

		EndDrawing();

		if (t->running)
			update_timer(t, GetFrameTime());
	}

	CloseWindow();

	return 0;
}
