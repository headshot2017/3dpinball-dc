#pragma once

#include <string>

class dc_graphics
{
public:
    static void Initialize();
    static void SwapBuffers();

	static void ShowSplash(std::string text);

	static void UpdateFull();
	static void Update();

private:
	static uint16_t* splash_raw;
};