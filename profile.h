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
 * File:   profile.h
 * Author: Daniel Machon
 *
 * Created on 29. marts 2020, 22:03
 */

#ifndef PROFILE_H
#define PROFILE_H

#include "io.h"

enum {
	PROFILE_ACTION_MOUSE = 0,
	PROFILE_ACTION_KEY = 1,
} profile_action;

struct profile_entry {
	struct io_event ev;
	int delay;
};

struct profile {
	const char *name;
	struct profile_entry entries[4096];
	int idx;
	int size;
};

/**
 * 
 * @param p
 * @return 
 */
int profile_create_loop(struct profile *p);

/**
 * 
 * @param p
 * @param path
 * @return 
 */
int profile_save(const struct profile *p, const char *path);

/**
 * 
 * @param path
 * @return 
 */
struct profile* profile_load(const char *path);

/**
 * 
 * @param p
 * @param ev
 * @param delay
 */
void profile_add_entry(struct profile *p,
	const struct io_event *ev, int delay);

/**
 * 
 * @param kb
 * @param delay
 */
void profile_print_kb_event(const struct io_event_kb *kb, int delay);

/**
 * 
 * @param mouse
 * @param delay
 */
void profile_print_mouse_event(const struct io_event_mouse *mouse, int delay);


#endif /* PROFILE_H */

