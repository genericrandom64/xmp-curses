#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <xmp.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <termios.h>

/* TODO
 * move all terminal stuff to a separate file
 * there is... quite a bit of code to clean up lmao
 */

#define SAMPLE_RATE 44100
#define MODULE queue[queue_index]

#define XC_WANT_BACKEND_AUDIO
#define XC_WANT_BACKEND_TERM
#include "config.h"

#ifndef XC_LANG
#error "No language selected in config.h?"
#endif

#ifndef XC_BACKEND_TERM
#error "No terminal backend selected in config.h?"
#endif

#ifndef XC_BACKEND_AUDIO
#error "No audio backend selected in config.h?"
#endif

uint8_t server = 0, noout = 0;

void sighandle(int sig) {
	if(server) shm_unlink(SHM);
	if(!noout) termend();
	exit(sig >> 8);
}

void die(char * msg, int ret) {
	printf(msg);
	sighandle(0);
}

int main(int argc, char **argv) {

	signal(SIGINT, sighandle);
	char statusbar[20];

	char buffer[SAMPLE_RATE];
	char* serverctl;
	struct stat srvstat;

	int opt, player_flags = 0, input = 0, loop = 1, interp = XMP_INTERP_LINEAR, tracknum = 1, subsong = 0, serverfd, mapsize = 22;
	uint8_t sep = 70, pause = 0, exitloop = 0, iexit = 0, loop_playlist = 0, force = 0;

	xmp_context c = xmp_create_context();
	struct xmp_frame_info frame_info;
	struct xmp_module_info module_info;


	while((opt = getopt(argc, argv, ":i:s:z:LRDlhm8uFN")) != -1) {
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

		case 'R':
			loop_playlist = 1;
			break;
		case 'F':
			shm_unlink(SHM);
			break;
		case 'l':
			loop = 0;
			break;
		case 's':
			sep = atoi(optarg);
			if(sep <= 0 && sep >= 100) {
				sep = 70;
			}
			break;
		case 'i':
			if(strcmp(optarg, XMPCURSES_NEAREST) == 0) interp = XMP_INTERP_NEAREST;
			if(strcmp(optarg, XMPCURSES_SPLINE) == 0) interp = XMP_INTERP_SPLINE;
			break;
		case 'h':
			return !printf(XMPCURSES_HELP);
			break;
		case '?':
			printf(XMPCURSES_UNKNOWN_ARG, optopt);
			break;
		case 'z':
			subsong = atoi(optarg);
			break;
		case 'D':
			server = 1;
			break;
		case 'N':
			noout = 1;
			break;
		}
	}

	int queue_size = argc - optind, queue_index = 0;
	char** queue = calloc(argc - optind, sizeof(char *));

	for(int i = 0; i < argc-optind; i++) {
		queue[i] = malloc(strlen(argv[optind+i]) + 1);
		strcpy(queue[i], argv[optind+i]);
	}

	if(!queue_size) return !printf("no files given\n");

	if(server) {
		if(shm_open(SHM, O_RDWR, 0777) != -1) return !printf("shm already exists, exiting\n");
		serverfd = shm_open(SHM, O_RDWR|O_CREAT, 0777);
		ftruncate(serverfd, 22);
		serverctl = mmap(NULL, 10, PROT_READ | PROT_WRITE, MAP_SHARED, serverfd, 0);
		// this isnt *technically* needed but if write isnt possible this will crash
		// before we get to something like ncurses that could fuck up the terminal
		serverctl[0] = 0;
	}

	backend_init;

	if(!noout) terminit();

	while(queue_index < queue_size) {

		//printf("want module q%i (%s)\n", queue_index, MODULE);
		if(xmp_load_module(c, MODULE) != 0) {
			if(!noout) termerr();
			printf(XMPCURSES_LOAD_FAIL);
			queue_index++;
			continue;
		}

		xmp_get_module_info(c, &module_info);
		xmp_start_player(c, SAMPLE_RATE, player_flags);
		xmp_set_player(c, XMP_PLAYER_INTERP, interp);
		xmp_set_player(c, XMP_PLAYER_MIX, sep);
		subsong %= module_info.num_sequences;
		frame_info.sequence = subsong;
		xmp_set_position(c, module_info.seq_data[subsong].entry_point);

		player_loop:
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

				xmp_get_frame_info(c, &frame_info);
				// TODO change this
				if(exitloop == 1) break;

				backend_write;

			}

			if(server && serverctl[0] != 0) {
				if(serverctl[0] == 7) {
					munmap(serverctl, mapsize);
					ftruncate(serverfd, 22);
					mapsize = 22;
					serverctl = mmap(NULL, 22, PROT_READ | PROT_WRITE, MAP_SHARED, serverfd, 0);
					memcpy(serverctl + 1, statusbar, 20);
				} else if(serverctl[0] == 8) {
					munmap(serverctl, mapsize);
					mapsize = 2+strlen(MODULE);
					ftruncate(serverfd, mapsize);
					serverctl = mmap(NULL, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, serverfd, 0);
					memcpy(serverctl + 1, MODULE, strlen(MODULE) + 1);
				} else if(serverctl[0] == 9) {
					munmap(serverctl, mapsize);
					fstat(serverfd, &srvstat);
					mapsize = srvstat.st_size;
					serverctl = mmap(NULL, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, serverfd, 0);
					queue = reallocarray(queue, ++queue_size, sizeof(char*));
					queue[queue_size-1] = malloc(strlen(serverctl + 1));
					strcpy(queue[queue_size-1], serverctl + 1);
				}
				else input = serverctl[0];
				serverctl[0] = 0;
			} else input = getchar();

			eval:
			switch(input) {

			case XMPCURSES_QUIT:
				// waiting to see this on r/badcode
				iexit = 1;
				break;
			case XMPCURSES_PAUSE:
				pause ^= 1;
				break;
			case XMPCURSES_LOOP:
				loop = loop ? 0 : frame_info.loop_count + 1;
				break;
			case XMPCURSES_POS_PREV:
				xmp_prev_position(c);
				break;
			case XMPCURSES_MOD_PREV:
				queue_index -= 2;
				if(queue_index < -1) queue_index = (queue_size-2);
				exitloop = 1;
				break;
			case XMPCURSES_MOD_NEXT:
				if(loop_playlist && queue_index == queue_size - 1) queue_index = -1;
				exitloop = 1;
				break;
			case XMPCURSES_POS_NEXT:
				xmp_next_position(c);
				break;
			case XMPCURSES_LOOP_PLAYLIST:
				loop_playlist ^= 1;
				break;
			}

			sprintf(statusbar,
			        "%c%c%c\n%02d:%02d:%02d",
			        pause ? XMPCURSES_STATUS_PAUSE : ' ',
			        loop ? ' ' : XMPCURSES_STATUS_LOOP,
			        loop_playlist ? XMPCURSES_STATUS_LOOP_PLAYLIST : ' ',

			        // time into module
			        (int)((frame_info.time / 100) / (60 * 600)),
			        (int)(((frame_info.time / 100) / 600) % 60),
			        (int)(((frame_info.time / 100) / 10) % 60)
			       );

			if(!noout) termret();

			if(exitloop == 1 || iexit == 1) break;

		}

		if(iexit == 1) break;
		queue_index++;
		exitloop = 0;

	}

	if(loop_playlist == 1 && iexit != 1) {
		queue_index = -1;
		goto player_loop;
	}

	// we are done, free stuff and exit
	xmp_end_player(c);
	xmp_release_module(c);
	xmp_free_context(c);
	for(int i = 0; i < queue_size; i++) free(queue[i]);
	free(queue);
	backend_free;
	sighandle(0);

}
