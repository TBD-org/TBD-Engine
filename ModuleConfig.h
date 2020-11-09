#pragma once

#include "Module.h"
#include "ShortString.h"

struct SDL_version;

enum ScreenMode
{
	SM_WINDOWED,
	SM_BORDERLESS,
	SM_FULLSCREEN,
	SM_FULLSCREEN_DESKTOP
};

class ModuleConfig : public Module
{
public:
	ModuleConfig();
	~ModuleConfig();

	bool PostInit();
	update_status Update();
	bool CleanUp();

public:
	// Configuration

	ShortString<20> app_name = "Real Engine";
	ShortString<20> organization = "Jaime Bea";
	int max_fps = 60;
	bool limit_framerate = true;
	bool vsync = true;
	int screen_mode = SM_WINDOWED;
	bool resizable = true;
	int screen_width = 640;
	int screen_height = 480;
	float brightness = 1.0f;

	// Information

	ShortString<20> sdl_version = "Not available";
	int cpu_count = 0;
	int cache_size_kb = 0;
	float ram_gb = 0;
	bool caps[14] = { false };
	const char* gpu_vendor = "Not available";
	const char* gpu_renderer = "Not available";
	const char* gpu_opengl_version = "Not available";
	float vram_budget_mb = 0;
	float vram_available_mb = 0;
	float vram_usage_mb = 0;
	float vram_reserved_mb = 0;
};

