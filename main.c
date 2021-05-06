#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <xmp.h>
#include <stdint.h>
#include <getopt.h>
#include <curses.h>
#include <string.h>
#include <assert.h>

#define SAMPLE_RATE 44100
#define MODULE argv[optind]

#include "config.h"

#ifndef XC_LANG
#error "No language selected in config.h?"
#endif

#ifndef XC_BACKEND
#error "No audio backend selected in config.h?"
#endif

void die(char * msg, int ret) {
	// TODO free audio
	clear();
	refresh();
	endwin();
	printf(msg);
	exit(ret);
}

int main(int argc, char **argv) {

	char statusbar[20];

	// this can go back once proper exit is
	// ready in case some platform doesnt
	// properly deallocate heap memory 
	//char* buffer = malloc(SAMPLE_RATE);
	char buffer[SAMPLE_RATE];
	
	int opt, player_flags = 0, input = 0, loop = 1, interp = XMP_INTERP_LINEAR, tracknum = 1;
	uint8_t sep = 70, pause = 0, exitloop = 0, exit = 0;
	
	// set up libxmp
	
	xmp_context c = xmp_create_context();
	struct xmp_frame_info frame_info;
	struct xmp_module_info module_info;

	
	while((opt = getopt(argc, argv, ":i:s:lm8u")) != -1) {
		switch(opt) {
			
			/* use these flags if you hate yourself and want to die
			 * todo make this work with pulseaudio
			 * and also every future backend
			case 'm':
				player_flags = player_flags|XMP_FORMAT_MONO;
				break;
			case '8':
				player_flags = player_flags|XMP_FORMAT_8BIT;
				break;
			case 'u':
				player_flags = player_flags|XMP_FORMAT_UNSIGNED;
				break;
			*/

			case 'l':
				loop = 0;
				break;
			case 's':
				sep = atoi(optarg);
				if(sep <= 0 && sep >= 100) { sep = 70; }
				break;
			case 'i':
				if(strcmp(optarg, XMPCURSES_NEAREST) == 0) interp = XMP_INTERP_NEAREST;
				if(strcmp(optarg, XMPCURSES_SPLINE) == 0) interp = XMP_INTERP_SPLINE;
				break;
			case 'h':
				printf(XMPCURSES_HELP);
				break;
			case '?':
				printf(XMPCURSES_UNKNOWN_ARG, optopt);
				break;
		}
	}

	tracknum = optind;
	
	/*if(xmp_load_module(c, MODULE) != 0) die(XMPCURSES_LOAD_FAIL, 1);
	xmp_get_module_info(c, &module_info);
	xmp_start_player(c, SAMPLE_RATE, player_flags);
	xmp_set_player(c, XMP_PLAYER_INTERP, interp);
	xmp_set_player(c, XMP_PLAYER_MIX, sep);*/
	// libxmp setup done

	backend_init;

	// set up ncurses
	initscr();
	cbreak();
	noecho();
	clear();
	curs_set(0);
	halfdelay(1);
	//nodelay(stdscr, 1);
	// ncurses setup done

	while(optind < argc) {
	if(xmp_load_module(c, MODULE) != 0) die(XMPCURSES_LOAD_FAIL, 1);
	xmp_get_module_info(c, &module_info);
	xmp_start_player(c, SAMPLE_RATE, player_flags);
	xmp_set_player(c, XMP_PLAYER_INTERP, interp);
	xmp_set_player(c, XMP_PLAYER_MIX, sep);
	// player loop
	while(1) {

		if(pause == 0) { 
			switch(xmp_play_buffer(c, buffer, SAMPLE_RATE, loop)) {
			
				case 0-XMP_END: 
					exitloop = 1;
					break;

				case 0-XMP_ERROR_STATE:
					die(XMPCURSES_INT_ERR, 1);
					break;

			}

			// if you do not have this line you will *suffer*
			xmp_get_frame_info(c, &frame_info);
			// change this
			if(exitloop == 1) break;

			backend_write;

		}

		input = getch();

		switch(input) {
		
			// TODO localize
			case 'q'://113
				// waiting to see this on r/badcode
				exit = 1;
				break;
			case ' ':
				if(pause == 0) pause = 1;
				else pause = 0;
				break;
			case 'L':
				if(loop == 0) loop = frame_info.loop_count + 1;
				else loop = 0;
				break;
			case 'h':
				xmp_prev_position(c);
				break;
			case 'j':
				// hacky function
				optind -= 2;
				if(optind < tracknum) optind = (tracknum-1); // subtract 1 because loop increments optind
				exitloop = 1;
				break;
			case 'k':
				exitloop = 1;
				break;
			case 'l':
				xmp_next_position(c);
				break;
				
			/*default:
				printf("\n%i\n", input);*/
		
		}
		
		sprintf(statusbar,
			"%c%c\n%02d:%02d:%02d",
			pause ? XMPCURSES_STATUS_PAUSE : ' ',
			loop ? ' ' : XMPCURSES_STATUS_LOOP,

			// time into module
			(int)((frame_info.time / 100) / (60 * 600)),
			(int)(((frame_info.time / 100) / 600) % 60),
			(int)(((frame_info.time / 100) / 10) % 60)
			);
		mvaddstr(0, 0, statusbar);
		refresh();
		if(exitloop == 1 || exit == 1) break;

	}
	optind++;
	exitloop = 0;
	if(exit == 1) break;
	}

	// we are done, free stuff
	xmp_end_player(c);
	xmp_release_module(c);
	xmp_free_context(c);

	backend_free;

	// we are done with ncurses
	clear();
	refresh();
	endwin();

	return 0;

}
