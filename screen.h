/*
 * Copyright (C) 2020 Daniel Machon
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/* 
 * File:   pixel.h
 * Author: Daniel Machon
 *
 * Created on 29. marts 2020, 22:18
 */

#ifndef PIXEL_H
#define PIXEL_H

struct screen {
	DWORD* pixels; /* Pointer to raw bitmap bits. Access with: pixels[(y * Screen.cx) + x] */
	size_t cx, cy; /* Width and height of bitmap. */
	HBITMAP hBmp;
	HDC hdcMem;
	HDC hdcScreen;
};

/**
 * 
 * @param s
 */
void screen_copy_pixels(struct screen *s);

/**
 * 
 * @return 
 */
struct screen* screen_init(void);

/**
 * 
 * @param screen
 * @param x
 * @param y
 * @return 
 */
COLORREF screen_get_pixel_color(const struct screen *screen, int x, int y);

/**
 * 
 * @param color
 * @param buf
 * @param size
 */
void screen_get_pixel_rgb_str(COLORREF color, char *buf, int size);

#endif /* PIXEL_H */

