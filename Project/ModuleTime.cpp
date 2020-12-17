#include "ModuleTime.h"

#include "Globals.h"
#include "Logging.h"
#include "Application.h"
#include "ModuleScene.h"

#include "SDL_timer.h"

#include "Leaks.h"

ModuleTime::ModuleTime()
{
	timer.Start();
}

UpdateStatus ModuleTime::PreUpdate()
{
	frame_count += 1;

	unsigned int real_time = timer.Read();
	real_time_delta_ms = real_time - real_time_last_ms;
	real_time_last_ms = real_time;

	if (game_running)
	{
		time_delta_ms = lroundf(real_time_delta_ms * time_scale);
		time_last_ms += time_delta_ms;
	}
	else if (game_step_once)
	{
		time_delta_ms = step_delta_time_ms;
		time_last_ms += time_delta_ms;

		game_step_once = false;
	}
	else
	{
		time_delta_ms = 0;
	}

	log_delta_ms((float)real_time_delta_ms);

    return UpdateStatus::CONTINUE;
}

void ModuleTime::WaitForEndOfFrame()
{
	if (limit_framerate)
	{
		unsigned int real_time_ms = timer.Read();
		unsigned int frame_ms = real_time_ms - real_time_last_ms;
		unsigned int min_ms = 1000 / max_fps;
		if (frame_ms < min_ms)
		{
			SDL_Delay(min_ms - frame_ms);
		}
	}
}

float ModuleTime::GetDeltaTime() const
{
	return time_delta_ms / 1000.0f;
}

float ModuleTime::GetRealTimeDeltaTime() const
{
	return real_time_delta_ms / 1000.0f;
}

float ModuleTime::GetTimeSinceStartup() const
{
	return time_last_ms / 1000.0f;
}

float ModuleTime::GetRealTimeSinceStartup() const
{
	return real_time_last_ms / 1000.0f;
}

float ModuleTime::GetTimeScale() const
{
	return time_scale;
}

void ModuleTime::SetTimeScale(float time_scale)
{
	time_scale = std::max(0.0f, time_scale);
}

bool ModuleTime::HasGameStarted() const
{
	return game_started;
}

bool ModuleTime::IsGameRunning() const
{
	return game_running;
}

void ModuleTime::StartGame()
{
	if (game_started) return;
	
	game_started = true;
	game_running = true;
}

void ModuleTime::StopGame()
{
	if (!game_started) return;

	game_started = false;
	game_running = false;

	time_last_ms = 0;
	// TODO: Restore the scene
}

void ModuleTime::PauseGame()
{
	if (!game_started) return;
	if (!game_running) return;

	game_running = false;
}

void ModuleTime::ResumeGame()
{
	if (!game_started) return;
	if (game_running) return;

	game_running = true;
}

void ModuleTime::StepGame()
{
	if (!game_started) StartGame();
	if (game_running) PauseGame();

	game_step_once = true;
}

unsigned int ModuleTime::GetFrameCount() const
{
	return frame_count;
}