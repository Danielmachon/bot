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

#include <stdio.h>
#include <windows.h>
#include <sys/time.h>

#include "profile.h"
#include "screen.h"
#include "constants.h"
#include "util.h"

enum {
	STM_IDLE = 0,
	STM_QUEUED = 1,
	STM_IN_BG = 2,
	STM_LEAVE = 3,
};

static void run_entry(const struct profile_entry *e)
{
	if (e->delay)
		Sleep(e->delay / 1000);

	if (e->ev.type == INPUT_KEYBOARD)
		io_simulate_kb_input(&e->ev.event.kb);
	else if (e->ev.type == INPUT_MOUSE)
		io_simulate_mouse_input(&e->ev.event.mouse);
}

static void run_profile(struct profile *p)
{
	int x;
	for (x = 0; x < p->size; x++) {
		const struct profile_entry *e =
			&p->entries[p->idx++];
		run_entry(e);
	}
}

static int has_defect(const struct screen *s)
{
	COLORREF c = screen_get_pixel_color(s, BG_DEFECT_COORD_X,
		BG_DEFECT_COORD_Y);
	if (c == BG_DEFECT_COLOR_TRIGGER) {
		return 1;
	}
	return 0;
}

static int is_queued_or_in_bg(const struct screen *s)
{
	COLORREF c = screen_get_pixel_color(s, BG_ICON_COORD_X,
		BG_ICON_COORD_Y);
	if (c == BG_ICON_COLOR_TRIGGER)
		return 1;
	return 0;
}

static int is_ready_to_enter(const struct screen *s)
{

	COLORREF c = screen_get_pixel_color(s, BG_POPUP_COORD_X,
		BG_POPUP_COORD_Y);
	if (c == BG_POPUP_COLOR_TRIGGER)
		return 1;
	return 0;
}

static int is_bg_over(const struct screen *s)
{
	COLORREF c1 = screen_get_pixel_color(s, BG_LEAVE_CONDITION1_X,
		BG_LEAVE_CONDITION1_Y);
	COLORREF c2 = screen_get_pixel_color(s, BG_LEAVE_CONDITION2_X,
		BG_LEAVE_CONDITION2_Y);
	if (c1 == BG_LEAVE_CONDITION1_COLOR_TRIGGER &&
		c2 == BG_LEAVE_CONDITION2_COLOR_TRIGGER)
		return 1;
	return 0;
}

static void press_keys(const int keys[])
{
	struct io_event_kb kb;
	int x = 0;
	while (keys[x] != 0x00) {
		kb.key = keys[x++];
		kb.flag = 0x00;
		io_simulate_kb_input(&kb);
		kb.flag = KEYEVENTF_KEYUP;
		io_simulate_kb_input(&kb);
	}
}

static void mouse_input(int flag, int x, int y)
{
	struct io_event_mouse m;
	m.flag = flag;
	m.x = x;
	m.y = y;
	io_simulate_mouse_input(&m);
}

static void anti_afk()
{
	static struct timeval s, e;

	gettimeofday(&e, NULL);
	if (delta(&s, &e) < 10000000)
		return;

	int x = (rand() % ((int) (GetSystemMetrics(SM_CXSCREEN) * 1.5) - 1000 + 1)) + 1000;
	int y = (rand() % ((int) (GetSystemMetrics(SM_CYSCREEN) * 1.5) - 500 + 1)) + 500;
	mouse_input(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, x, y);

	gettimeofday(&s, NULL);
}

static void talk_to_npc()
{
	mouse_input(MOUSEEVENTF_RIGHTDOWN, BG_NPC_COORD_X,
		BG_NPC_COORD_Y);
	mouse_input(MOUSEEVENTF_RIGHTUP, BG_NPC_COORD_X,
		BG_NPC_COORD_Y);
	Sleep(1000);
	mouse_input(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, BG_NPC_DIALOG1_X,
		BG_NPC_DIALOG1_Y);
	mouse_input(MOUSEEVENTF_LEFTDOWN, BG_NPC_DIALOG1_X,
		BG_NPC_DIALOG1_Y);
	mouse_input(MOUSEEVENTF_LEFTUP, BG_NPC_DIALOG1_X,
		BG_NPC_DIALOG1_Y);
}

const int battlefield_frame_join_button_keys[] = {
	0x0d, 0x6f, 0x43, 0x4c, 0x49, 0x43, 0x4b, 0x20, 0x14, 0x42, 0x14,
	0x41, 0x54, 0x54, 0x4c, 0x45, 0x46, 0x49, 0x45, 0x4c, 0x44,
	0x14, 0x46, 0x14, 0x52, 0x41, 0x4d, 0x45, 0x14, 0x4a, 0x14,
	0x4f, 0x49, 0x4e, 0x14, 0x42, 0x14, 0x55, 0x54, 0x54, 0x4f,
	0x4e, 0x0d, 0x00
};

const int static_popup1_button1[] = {
	0x0d, 0x6f, 0x43, 0x4c, 0x49, 0x43, 0x4b, 0x20, 0x14, 0x53, 0x14,
	0x54, 0x41, 0x54, 0x49, 0x43, 0x14, 0x50, 0x14, 0x4f, 0x50,
	0x55, 0x50, 0x31, 0x14, 0x42, 0x14, 0x55, 0x54, 0x54, 0x4f,
	0x4e, 0x31, 0x0d, 0x00
};

const int world_state_score_frame_leave_button[] = {
	0x0d, 0x6f, 0x43, 0x4c, 0x49, 0x43, 0x4b, 0x20, 0x14, 0x57, 0x14,
	0x4f, 0x52, 0x4c, 0x44, 0x14, 0x53, 0x14, 0x54, 0x41, 0x54,
	0x45, 0x14, 0x53, 0x14, 0x43, 0x4f, 0x52, 0x45, 0x14, 0x46,
	0x14, 0x52, 0x41, 0x4d, 0x45, 0x14, 0x4c, 0x14, 0x45, 0x41,
	0x56, 0x45, 0x14, 0x42, 0x14, 0x55, 0x54, 0x54, 0x4f, 0x4e,
	0x0d, 0x00
};

const int static_popup1_button2[] = {
	0x0d, 0x6f, 0x43, 0x4c, 0x49, 0x43, 0x4b, 0x20, 0x14, 0x53, 0x14,
	0x54, 0x41, 0x54, 0x49, 0x43, 0x14, 0x50, 0x14, 0x4f, 0x50,
	0x55, 0x50, 0x31, 0x14, 0x42, 0x14, 0x55, 0x54, 0x54, 0x4f,
	0x4e, 0x32, 0x0d, 0x00
};

const int escape[] = {
	0x1b, 0x00
};

void stm(struct profile *p, struct screen *s)
{
	static int state = STM_IN_BG;
	COLORREF c;

	switch (state) {
	case STM_IDLE:
	{
		if (has_defect(s)) {
			fprintf(stderr, "Defect.\n");
			break;
		} else {
			fprintf(stderr, "Idle.\n");
		}

		/**
		 * Queue up.
		 */
		talk_to_npc();
		press_keys(battlefield_frame_join_button_keys);
		Sleep(1000);
		press_keys(escape);

		state = STM_QUEUED;

		Sleep(5000);

		break;
	}
	case STM_QUEUED:
	{
		fprintf(stderr, "Queued.\n");

		/**
		 * Wait for pop-up.
		 */

		if (is_ready_to_enter(s)) {
			press_keys(static_popup1_button1);
			state = STM_IN_BG;
			/* Give some time to enter BG */
			Sleep(10000);
			break;
		}/* 
			 * If we for some reason has left the queue 
			 */
		else if (!is_queued_or_in_bg(s)) {
			state = STM_IDLE;
			break;
		}

		//anti_afk();

		break;
	}
	case STM_IN_BG:
	{
		fprintf(stderr, "Inside BG.\n");

		/* If we have been kicked */
		if (!is_queued_or_in_bg(s)) {
			state = STM_IDLE;
			break;
		}

		if (is_bg_over(s)) {
			state = STM_LEAVE;
			break;
		}

		/* Either run BG profile or Antiafk profile. */
		if (p->idx < p->size - 1)
			run_profile(p);
		else
			anti_afk();

		break;
	}
	case STM_LEAVE:
	{
		fprintf(stderr, "Leaving.\n");

		press_keys(world_state_score_frame_leave_button);
		press_keys(static_popup1_button1);

		state = STM_IDLE;
		break;
	}

	default:
		break;
	}

}