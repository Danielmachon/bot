/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Daniel Machon
 *
 * Created on 28. marts 2020, 10:52
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <getopt.h>

#include "screen.h"
#include "proc.h"
#include "profile.h"

struct profile *p;

void help()
{

}

void version()
{

}

int parse_argv(int argc, char **argv)
{

	enum {
		ARG_FAIL = 0x100,
		ARG_VERSION,
		ARG_HELP,
		ARG_CREATE_PROFILE,
		ARG_LOAD_PROFILE,
	};

	static const struct option options[] = {
		{ "help", no_argument, NULL, 'h'},
		{ "version", no_argument, NULL, 'v'},
		{ "create-profile", required_argument, NULL, 'c'},
		{ "profile", no_argument, NULL, 'p'},
		{ "bg", required_argument, NULL, 'b'},
		{ "dump", no_argument, NULL, 'd' },
		{}
	};

	int c;
	while ((c = getopt_long(argc, argv, "b:d:hc:p:v", options, NULL)) >= 0) {
		switch (c) {
		case '?':
			break;
		case 'b':


		case 'c':
		{
			struct profile p = {0};
			if (profile_create_loop(&p) < 0)
				fprintf(stderr, "Failed to create profile");
			profile_print(&p);
			profile_save((const struct profile *) &p, optarg);
			return 0;
		}
		case 'd': {
			DWORD pid;
			DWORD *addr;
			proc_get_id("WoWClassic.exe", &pid);
			addr = proc_get_base_address(pid);
			return 0;
		}
		case 'h':
			help();
			return 0;
		case 'p':
		{
			p = profile_load(optarg);
			break;
		}
		case 'v':
			return 0;
		case ARG_CREATE_PROFILE:
			break;
		case ARG_HELP:
			help();
			return 0;
		case ARG_VERSION:
			version();
			return 0;
		}
	}
	return 1;
}

/*
 * 
 */
int main(int argc, char** argv)
{
	fprintf(stderr, "Welcome!\n");

	if (!parse_argv(argc, argv))
		exit(EXIT_SUCCESS);

	struct screen *s = screen_init();

	Sleep(5000);

	for (;;) {
		screen_copy_pixels(s);
		stm(p, s);
		Sleep(1000);
	}

	return(EXIT_SUCCESS);
}

