#include <windows.h>
#include <stdio.h>

#include "screen.h"

/* Call once to setup.
 */
static void __screen_init(struct screen *s)
{
	BITMAPINFO bmp = {0};

	s->cx = GetSystemMetrics(SM_CXSCREEN);
	s->cy = GetSystemMetrics(SM_CYSCREEN);

	bmp.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmp.bmiHeader.biWidth = s->cx;
	bmp.bmiHeader.biHeight = -(int) s->cy;
	bmp.bmiHeader.biPlanes = 1;
	bmp.bmiHeader.biBitCount = 32;
	bmp.bmiHeader.biCompression = BI_RGB;

	s->hBmp = CreateDIBSection(NULL, &bmp, DIB_RGB_COLORS, (void**) &s->pixels, NULL, 0);
	s->hdcMem = CreateCompatibleDC(NULL);
	SelectObject(s->hdcMem, s->hBmp);

	s->hdcScreen = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
}

/* 
 * Call to copy the current screen into the screen bitmap.
 */
void screen_copy_pixels(struct screen *s)
{
	BitBlt(s->hdcMem, 0, 0, s->cx, s->cy, s->hdcScreen, 0, 0, SRCCOPY);
}

/* Convert a RGB value to the BGR used by Windows COLORREF values.
 */
static COLORREF RGB2BGR(DWORD color)
{
	return RGB(GetBValue(color), GetGValue(color), GetRValue(color));
}

struct screen* screen_init(void)
{
	struct screen *s = calloc(1, sizeof(struct screen));
	
	__screen_init(s); /* Call once for setup. */
	return s;
}

COLORREF screen_get_pixel_color(const struct screen *screen, int x, int y)
{
	return GetPixel(screen->hdcScreen, x, y); 
}

void screen_get_pixel_rgb_str(COLORREF color, char *buf, int size)
{
	snprintf(buf, size, "R:%u, G:%u, B:%u", 
		GetRValue(color), 
		GetGValue(color), 
		GetBValue(color));
}