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

#include "errno.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <windows.h>
#include <winbase.h>
#include <winnt.h>
#include <sys/unistd.h>
#include <limits.h>

#include "profile.h"
#include "io.h"
#include "screen.h"
#include "util.h"

int profile_handle_kb_input(const struct io_event *ev,
	struct profile *p);

int profile_handle_mouse_input(const struct io_event *ev,
	struct profile *p);

void profile_print_kb_event(const struct io_event_kb *kb,
	int delay);

void profile_print_mouse_event(const struct io_event_mouse *mouse,
	int delay);

int profile_save(const struct profile *p, const char *path)
{
	int size;
	FILE *f = fopen(path, "w+");

	if (!f) {
		fprintf(stderr, strerror(errno));
		return -1;
	}

	size = fwrite((void *) p, 1, sizeof(struct profile), f);

	if (size < sizeof(struct profile))
		return -1;

	return 0;
}

struct profile* profile_load(const char *path)
{
	int size;
	struct profile *p = calloc(1, sizeof(struct profile));

	FILE *f = fopen(path, "r+");

	if (!f) {
		fprintf(stderr, strerror(errno));
		return NULL;
	}

	size = fread((void *) p, 1, sizeof(struct profile), f);

	if (size < sizeof(struct profile))
		return NULL;

	return p;
}

void profile_print(const struct profile *p)
{
	int x;
	for (x = 0; x < p->size; x++) {
		const struct profile_entry *e = &p->entries[x];
		if (e->ev.type == INPUT_KEYBOARD)
			profile_print_kb_event(&e->ev.event.kb, e->delay);
		else if (e->ev.type == INPUT_MOUSE)
			profile_print_mouse_event(&e->ev.event.mouse, e->delay);
	}
}

void profile_print_kb_event(const struct io_event_kb *kb, int delay)
{
	fprintf(stderr,
		"[%u,%u,%u,%u]\n",
		INPUT_KEYBOARD,
		kb->key,
		kb->flag,
		delay);
}

void profile_print_mouse_event(const struct io_event_mouse *mouse, int delay)
{
	fprintf(stderr,
		"[%u,%u,%u,%u,%u]\n",
		INPUT_MOUSE,
		mouse->x,
		mouse->y,
		mouse->flag,
		delay);
}

void profile_add_entry(struct profile *p,
	const struct io_event *ev, int delay)
{
	struct profile_entry e = {
		.ev = *ev,
		.delay = delay
	};

	p->entries[p->size++] = e;
}

int key_pressed;
int key_released;
#define KEY_PRESSED() ({key_pressed = 1; key_released = 0;})
#define KEY_RELEASED() ({key_pressed = 0; key_released = 1;})
struct timeval last_event_time;
struct timeval event_time;
int keys[0xFE] = {0};

int profile_create_loop(struct profile *p)
{
	int start = 0;

	struct timeval tv = {0, 1000};

	fd_set readfds;
	SetWindowsHookEx(WH_KEYBOARD_LL, io_kb_logger, 0, 0);
	SetWindowsHookEx(WH_MOUSE_LL, io_mouse_logger, 0, 0);

	if (pipe(keyboard_pipe) < 0)
		return -1;

	MSG msg;
	while (1) {

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			continue;

		struct io_event ev;
		const struct io_event_kb *ev_kb;
		const struct io_event_mouse *ev_mouse;

		FD_ZERO(&readfds);
		FD_SET(keyboard_pipe[0], &readfds);

		int num;
		if ((num = select(FD_SETSIZE,
			&readfds, NULL, NULL, &tv)) < 0) {
			return -1;
		}

		if (FD_ISSET(keyboard_pipe[0], &readfds))
			read(keyboard_pipe[0], &ev, sizeof(struct io_event));
		else
			continue;

		ev_kb = &ev.event.kb;
		ev_mouse = &ev.event.mouse;

		/* Wait for start key */
		if (!start) {
			if (ev.type == INPUT_KEYBOARD &&
				ev_kb->key == 0x70) {
				start = 1;
				gettimeofday(&last_event_time, NULL);
				continue;
			} else {
				continue;
			}
		} else {
			if (ev.type == INPUT_KEYBOARD &&
				ev_kb->key == 0x70)
				continue;
			if (ev.type == INPUT_KEYBOARD &&
				ev_kb->key == 0x71) {
				fprintf(stderr, "Stopping ..\n");
				return 0;
			}
		}

		switch (ev.type) {
		case INPUT_MOUSE:
			profile_handle_mouse_input((const struct io_event *) &ev, p);
			break;
		case INPUT_KEYBOARD:
			profile_handle_kb_input((const struct io_event *) &ev, p);
			break;
		}

	}
}

int profile_handle_kb_input(const struct io_event *ev,
	struct profile *p)
{
	const struct io_event_kb *k = &ev->event.kb;
	int key_flag = io_wm_to_eventf(k->flag);
	int key = k->key;

	gettimeofday(&event_time, NULL);

	switch (key_flag) {
	case 0x00:
	{
		if (keys[key])
			break;

		fprintf(stderr, "Key: %x, flag: %s\n",
			k->key,
			io_eventf_to_str(io_wm_to_eventf(k->flag)));
		++keys[key];
		profile_add_entry(p, ev, delta(&last_event_time, &event_time));
		gettimeofday(&last_event_time, NULL);

		break;
	}
	case KEYEVENTF_KEYUP:
	{
		fprintf(stderr, "Key: %x, flag: %s\n",
			k->key,
			io_eventf_to_str(io_wm_to_eventf(k->flag)));
		--keys[key];
		profile_add_entry(p, ev, delta(&last_event_time, &event_time));
		memcpy(&last_event_time, &event_time, sizeof(struct timeval));
		break;
	}
	}
}

int profile_handle_mouse_input(const struct io_event *ev,
	struct profile *p)
{
	const struct io_event_mouse *m = &ev->event.mouse;
	int x = m->x;
	int y = m->y;
	int flag = m->flag;

	char color_buf[64] = {0};
	struct screen *s = screen_init();
	screen_copy_pixels(s);
	COLORREF c = screen_get_pixel_color(s, x, y);
	screen_get_pixel_rgb_str(c, color_buf, sizeof(color_buf));

	fprintf(stderr, "Mouse %u: (%u, %u) [%s] {%u}\n", flag, x, y, color_buf, c);

	gettimeofday(&event_time, NULL);
	profile_add_entry(p, ev, delta(&last_event_time, &event_time));
	memcpy(&last_event_time, &event_time, sizeof(struct timeval));
}