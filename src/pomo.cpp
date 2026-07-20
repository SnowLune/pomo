#include <iostream>
#include <array>
#include <chrono>
#include <thread>
#include <string>

#include <raylib.h>

const int fps_target = 60;

constexpr int work_interval_s =		25 * 60;
constexpr int short_break_interval_s =	5 * 60;
constexpr int long_break_interval_s =	30 * 60;

// Durations for Work, Short Break, Long Break
const std::array<int, 3> interval_s = { 
	work_interval_s,
	short_break_interval_s,
	long_break_interval_s
};

const std::array<const char*, 3> phase_names = {
	"WORK",
	"SHORT BREAK",
	"LONG BREAK"
};

enum class Phase
{
	work = 0, short_break = 1, long_break = 2
};

struct Timer
{
	Sound	tick_sound;
	Sound	ring_sound;
	float		current_time_s;
	int		total_time_s;
	int		work_count;
	const char* 	phase_name;
	Phase		phase;
	bool		tick_muted;
	bool		running;
};

void
set_timer (Timer* t, Phase phase)
{
	const int phase_int = static_cast<int>(phase);
	
	t->phase = phase;
	t->phase_name = phase_names.at(phase_int);
	t->total_time_s = interval_s.at(phase_int);
	t->current_time_s = 0.0;
}

Timer
create_timer (const char* tick_sndfile, const char* ring_sndfile)
{
	Timer t {};
	
	// Initialize
	set_timer(&t, Phase::work);
	t.running = false;
	t.work_count = 0;
	
	// Load Sounds
	if (IsAudioDeviceReady())
	{
		t.tick_sound = LoadSound(tick_sndfile);
		t.ring_sound = LoadSound(ring_sndfile);
	}
	t.tick_muted = false;

	const float tick_vol = 0.1;
	SetSoundVolume(t.tick_sound, tick_vol);

	return t;
}

void
update_timer (Timer* t, float ds)
{
	t->current_time_s += ds;
	
	if ((int)t->current_time_s >= t->total_time_s)
	{
		if (t->phase == Phase::work)
		{
			if (t->work_count && t->work_count % 4 == 0)
				set_timer(t, Phase::long_break);
			else
				set_timer(t, Phase::short_break);
			
			++t->work_count;
		}
		else
			set_timer(t, Phase::work);

		// Play ring sound on interval completion
		if (!IsSoundPlaying(t->ring_sound)) PlaySound(t->ring_sound);
	}
	else
	{
		if (!t->tick_muted && !IsSoundPlaying(t->tick_sound))
			PlaySound(t->tick_sound);
	}
}

void
stop_timer (Timer* t)
{
	t->running = false;
}

void
mute_timer_tick (Timer* t)
{
	t->tick_muted = !t->tick_muted;
}

int
main (void)
{
	const int screen_width = 640;
	const int screen_height = 480;

	const float default_font_size = 32;
	const float default_font_spacing = 0;

	const char* sound_tick_file = "./sounds/timertick.ogg";
	const char* sound_ring_file = "./sounds/timerring.ogg";

	const char* font_file = "./fonts/JetBrainsMonoNerd.ttf";

	InitAudioDevice();
	InitWindow(screen_width, screen_height, "pomo");
	SetTargetFPS(fps_target);

	Font mono_font = LoadFont(font_file);
	
	Timer pomodoro = create_timer(sound_tick_file, sound_ring_file);
	Timer* t = &pomodoro;
	t->running = true;

	while (!WindowShouldClose())
	{
		std::string status_text
			= std::format(	"{} - {:8.2f}/{}",
					t->phase_name,
					t->current_time_s,
					t->total_time_s	);

		BeginDrawing();

		ClearBackground(RAYWHITE);

		const Vector2 text_pos = {10, 10};
		float font_size = default_font_size;
		float font_spacing = default_font_spacing;
		DrawTextEx(	mono_font,
				status_text.c_str(),
				text_pos,
				font_size,
				font_spacing,
				BLACK	);

		EndDrawing();

		if (t->running)
			update_timer(t, GetFrameTime());
	}

	CloseWindow();

	return 0;
}
