#include "dc_graphics.h"
#include "pch.h"
#include "maths.h"
#include "render.h"

#include <cstdio>
#include <cstring>
#include <malloc.h>

uint16_t* dc_graphics::splash_raw = 0;

void dc_graphics::Initialize()
{
	vid_set_mode(DM_640x480, PM_RGB565);

	// splash image is 320x222 RGB565
	splash_raw = new uint16_t[320*222];

	FILE* f = fopen("/rd/splash.raw", "rb");
	fread(splash_raw, 320*222*2, 1, f);
	fclose(f);
}

void dc_graphics::SwapBuffers()
{
	vid_waitvbl();
}

void dc_graphics::ShowSplash(std::string text)
{
	vid_clear(0,0,0);

	int startX = (vid_mode->width/2 - 320/2);
	int startY = (vid_mode->height/2 - 222/2);
	int startInd = (startY * vid_mode->width + startX);

	for (int y=0; y<222; y++)
	{
		for (int x=0; x<320; x++)
		{
			vram_s[startInd + y*vid_mode->width+x] = splash_raw[y*320+x];
		}
	}

	int y = startY+222+4;
	int textWidth = 0, x = 0;

	// determine text width first
	for (uint32_t i=0; i<text.size(); i++)
	{
		if (text.at(i) == '\n')
		{
			x = 0;
			continue;
		}

		x += 12;
		if (x > textWidth) textWidth = x;
	}

	// now we can draw
	startX = (vid_mode->width/2 - textWidth/2);
	x = startX;
	for (uint32_t i=0; i<text.size(); i++)
	{
		if (text.at(i) == '\n')
		{
			x = startX;
			y += 24;
			continue;
		}

		bfont_draw(vram_s + y*vid_mode->width+x, vid_mode->width, 1, text.at(i));
		x += 12;
	}
}

void dc_graphics::UpdateFull()
{
	int startPos = (vid_mode->height/2 - render::vscreen->Height/2) * vid_mode->width + (vid_mode->width/2 - render::vscreen->Width/2);

	for (int y=0; y<render::vscreen->Height; y++)
	{
		for (int x=0; x<render::vscreen->Width; x++)
		{
			Rgba c = render::vscreen->BmpBufPtr1[y*render::vscreen->Width+x].rgba;
			vram_s[startPos + y*vid_mode->width+x] =
				((c.Red >> 3))
				| ((c.Green & 0xFC) << 3)
				| ((c.Blue & 0xF8) << 8);
		}
	}
}

void dc_graphics::Update()
{
	int startPos = (vid_mode->height/2 - render::vscreen->Height/2) * vid_mode->width + (vid_mode->width/2 - render::vscreen->Width/2);

	for (uint32_t i=0; i<render::get_dirty_regions().size(); i++)
	{
		const rectangle_type& dirty = render::get_dirty_regions()[i];
		for (int y=dirty.YPosition; y<dirty.YPosition + dirty.Height; y++)
		{
			for (int x=dirty.XPosition; x<dirty.XPosition + dirty.Width; x++)
			{
				Rgba c = render::vscreen->BmpBufPtr1[y*render::vscreen->Width+x].rgba;
				vram_s[startPos + y*vid_mode->width+x] =
					((c.Red >> 3))
					| ((c.Green & 0xFC) << 3)
					| ((c.Blue & 0xF8) << 8);
			}
		}
	}

	render::get_dirty_regions().clear();
}
