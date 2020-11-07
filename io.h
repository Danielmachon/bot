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
 * File:   io.h
 * Author: Daniel Machon
 *
 * Created on 2. april 2020, 12:56
 */

#ifndef IO_H
#define IO_H

#include <fcntl.h>

/*
 * Pipe to communicate between callback functions and main loop.
 */
int keyboard_pipe[2];

/*
 * Struct encapsulating information about a key press.
 */
struct io_event_kb {
	int key;
	int flag;
};

struct  io_event_mouse {
	int flag;
	int x;
	int y;
};

struct io_event {
	int type;
	union {
		struct io_event_kb kb;
		struct io_event_mouse mouse;
	} event;
};

/**
 * Keyboard logger.
 * 
 * @param nCode
 * @param wParam
 * @param lParam
 * @return 
 */
LRESULT CALLBACK io_kb_logger(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * Mouse logger.
 * 
 * @param nCode
 * @param wParam
 * @param lParam
 * @return 
 */
LRESULT CALLBACK io_mouse_logger(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);

/**
 * Simulate input.
 * 
 * @param type
 * @param key
 * @param flags
 * @return 
 */
int io_simulate_input(DWORD type, WORD key, DWORD flags);

/**
 * Convert event flag to human-readable string.
 * 
 * @param flag
 * @return 
 */
const char* io_eventf_to_str(int flag);

/**
 * Convert windows message to event flag.
 * 
 * @param wm
 * @return 
 */
int io_wm_to_eventf(int wm);

/**
 * 
 * @param kb
 * @return 
 */
int io_simulate_kb_input(const struct io_event_kb *kb);

/**
 * 
 * @param m
 * @return 
 */
int io_simulate_mouse_input(const struct io_event_mouse *m);


#endif /* IO_H */

