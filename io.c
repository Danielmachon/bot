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
#include <unistd.h>

#include "profile.h"
#include "io.h"

const char* io_eventf_to_str(int flag)
{
	switch (flag) {
	case KEYEVENTF_KEYUP:
		return "Release";
	case 0x00:
		return "Press";
	default:
		return "Unknown";
	}
}

int io_wm_to_eventf(int wm)
{
	switch (wm) {
	case WM_KEYUP:
		return KEYEVENTF_KEYUP;
	case WM_KEYDOWN:
		return 0x00;
	case WM_MOUSEMOVE:
		return MOUSEEVENTF_MOVE;
	case WM_LBUTTONDOWN:
		return MOUSEEVENTF_LEFTDOWN;
	case WM_LBUTTONUP:
		return MOUSEEVENTF_LEFTUP;
	case WM_LBUTTONDBLCLK:
		return -1;
		;
	case WM_RBUTTONDOWN:
		return MOUSEEVENTF_RIGHTDOWN;
	case WM_RBUTTONUP:
		return MOUSEEVENTF_RIGHTUP;
	case WM_RBUTTONDBLCLK:
		return -1;
	case WM_MBUTTONDOWN:
		return MOUSEEVENTF_MIDDLEDOWN;
	case WM_MBUTTONUP:
		return MOUSEEVENTF_MIDDLEUP;
	case WM_MBUTTONDBLCLK:
		return -1;
	case WM_MOUSEWHEEL:
		return MOUSEEVENTF_WHEEL;
	case WM_XBUTTONDOWN:
		return MOUSEEVENTF_XDOWN;
	case WM_XBUTTONUP:
		return MOUSEEVENTF_XUP;
	case WM_XBUTTONDBLCLK:
		return -1;
		;
	default:
		-1;
	}
}

int io_simulate_kb_input(const struct io_event_kb *kb)
{
	// This structure will be used to create the keyboard
	// input event.
	INPUT ip;

	// Set up a generic keyboard event.
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; // hardware scan code for key
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	// Press key
	ip.ki.wVk = kb->key; // virtual-key code for the "a" key
	ip.ki.dwFlags = io_wm_to_eventf(kb->flag); // 0 for key press

	SendInput(1, &ip, sizeof(INPUT));
}

int io_simulate_mouse_input(const struct io_event_mouse *m)
{
	INPUT ip = {0};

	ip.type = INPUT_MOUSE;

	ip.mi.dx = (m->x * 65536.0f) / (GetSystemMetrics(SM_CXSCREEN) * 1.5);
	ip.mi.dy = (m->y * 65536.0f) / (GetSystemMetrics(SM_CYSCREEN) * 1.5);

	ip.mi.dwFlags = io_wm_to_eventf(m->flag) | MOUSEEVENTF_ABSOLUTE;

	ip.mi.mouseData = 0;
	ip.mi.time = 0;
	ip.mi.dwExtraInfo = GetMessageExtraInfo();

	profile_print_mouse_event(m, 0);

	SendInput(1, &ip, sizeof(INPUT));
}


//The function that implements the key logging functionality

LRESULT CALLBACK io_kb_logger(int n_code,
	WPARAM w_param,
	LPARAM l_param)
{
	struct io_event p = {
		.type = INPUT_KEYBOARD,
		.event.kb.key = ((KBDLLHOOKSTRUCT *) l_param)->vkCode,
		.event.kb.flag = w_param
	};

	write(keyboard_pipe[1], (char *) &p, sizeof(struct io_event));

	//according to winapi all functions which implement a hook must return by calling next hook
	return CallNextHookEx(NULL, n_code, w_param, l_param);
}

LRESULT CALLBACK io_mouse_logger(
	_In_ int n_code,
	_In_ WPARAM w_param,
	_In_ LPARAM l_param
	)
{
	struct io_event p = {
		.type = INPUT_MOUSE,
		.event.mouse.flag = w_param,
		.event.mouse.x = ((MOUSEHOOKSTRUCT *) l_param)->pt.x,
		.event.mouse.y = ((MOUSEHOOKSTRUCT *) l_param)->pt.y,
	};

	write(keyboard_pipe[1], (char *) &p, sizeof(struct io_event));

	//according to winapi all functions which implement a hook must return by calling next hook
	return CallNextHookEx(NULL, n_code, w_param, l_param);
}