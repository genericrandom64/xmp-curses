#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <xmp.h>
#include <pulse/simple.h>
#include <curses.h>


#define SAMPLE_RATE 44100
#define MODULE argv[1] //"/mnt/en/generic/rt/world_of_plastic.s3m"

void die(char * msg, int ret) {
	clear();
	refresh();
	endwin();
	printf(msg);
	exit(ret);
}

int main(int argc, char **argv) {

	if(argc < 2) die("argv[1] must be a module name\n", 1);

	char statusbar[16];
	char* buffer = malloc(SAMPLE_RATE);
	int input, pause = 0, loop = 1, exit = 0;
	
	// set up libxmp
	xmp_context c = xmp_create_context();
	struct xmp_frame_info frame_info;
	if(xmp_load_module(c, MODULE) != 0) die("Could not load module.\n", 1);
	xmp_start_player(c, SAMPLE_RATE, 0);
	// libxmp setup done

	// set up pulse backend
	pa_simple *s;
	pa_sample_spec ss;
	 
	ss.format = PA_SAMPLE_S16NE;
	ss.channels = 2;
	ss.rate = SAMPLE_RATE;

	s = pa_simple_new(NULL, "xmp clone", PA_STREAM_PLAYBACK, NULL, basename(MODULE), &ss, NULL, NULL, NULL);
	// pulse setup done

	// set up ncurses
	initscr();
	cbreak();
	noecho();
	clear();
	curs_set(0);
	halfdelay(2);
	// ncurses setup done

	// player loop
	while(1) {

		if(pause == 0) { 
			switch(xmp_play_buffer(c, buffer, SAMPLE_RATE, loop)) {
			
				case 0-XMP_END: 
					exit = 1;
					break;

				case 0-XMP_ERROR_STATE:
					die("Internal error\n", 1);
					break;

			}

			// if you do not have this line you will *suffer*
			xmp_get_frame_info(c, &frame_info);
			// change this
			if(exit == 1) break;

			pa_simple_write(s, buffer, SAMPLE_RATE, 0);

		}

		input = getch();

		switch(input) {
		
			case 113:
				// waiting to see this on r/badcode
				exit = 1;
				break;
			case 32:
				if(pause == 0) pause = 1;
				else pause = 0;
				break;
			case 108:
				if(loop == 0) loop = frame_info.loop_count + 1;
				else loop = 0;
				break;
		
		}
		
		sprintf(statusbar, "%c%c", pause ? 'P' : ' ', loop ? ' ' : 'L');
		mvaddstr(0, 0, statusbar);
		if(exit == 1) break;

	}

	// we are done, free stuff
	xmp_end_player(c);
	xmp_release_module(c);
	xmp_free_context(c);
	pa_simple_free(s);

	// we are done with ncurses
	clear();
	refresh();
	endwin();

	return 0;

}
