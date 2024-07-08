#include "dc_graphics.h"
#include "pch.h"
#include "maths.h"
#include "render.h"

#include <cstdio>
#include <cstring>
#include <malloc.h>

void dc_graphics::Initialize()
{
	vid_set_mode(DM_640x480, PM_RGB565);
}

void dc_graphics::SwapBuffers()
{
	vid_waitvbl();
	vid_waitvbl();
}

void dc_graphics::UpdateFull()
{
	for (int y=0; y<render::vscreen->Height; y++)
	{
		for (int x=0; x<render::vscreen->Width; x++)
		{
			Rgba c = render::vscreen->BmpBufPtr1[y*render::vscreen->Width+x].rgba;
			vram_s[y*640+x] =
				((c.Red >> 3))
				| ((c.Green & 0xFC) << 3)
				| ((c.Blue & 0xF8) << 8);
		}
	}
}

void dc_graphics::Update()
{
	for (uint32_t i=0; i<render::get_dirty_regions().size(); i++)
	{
		const rectangle_type& dirty = render::get_dirty_regions()[i];
		for (int y=dirty.YPosition; y<dirty.YPosition + dirty.Height; y++)
		{
			for (int x=dirty.XPosition; x<dirty.XPosition + dirty.Width; x++)
			{
				Rgba c = render::vscreen->BmpBufPtr1[y*render::vscreen->Width+x].rgba;
				vram_s[y*640+x] =
					((c.Red >> 3))
					| ((c.Green & 0xFC) << 3)
					| ((c.Blue & 0xF8) << 8);
			}
		}
	}

	render::get_dirty_regions().clear();
}
