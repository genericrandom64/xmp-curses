#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// TODO obey language setting
#include "config.h"

#define addopt(I, O) case I: if(!out) out = O; break;
#define addpopt(I, O) case I: if(!out) out = O; packet = optarg; break;

int main(int argc, char **argv) {
	char out = 0;
	struct stat s;
	int opt;
	char* packet;
	char* help = "u [file] - add to queue\nm - show playing module\ns - get player status\nq - quit\np - pause\nl - loop one\nr - loop all\nd - back in module\nf - forward in module\nb - back one module\nn - next module\n";
	while((opt = getopt(argc, argv, ":u:sqplrbfdnm")) != -1) switch(opt) {
		addopt('q', XMPCURSES_QUIT);
		addopt('p', XMPCURSES_PAUSE);
		addopt('l', XMPCURSES_LOOP);
		addopt('r', XMPCURSES_LOOP_PLAYLIST);
		addopt('d', XMPCURSES_POS_PREV);
		addopt('f', XMPCURSES_POS_NEXT);
		addopt('b', XMPCURSES_MOD_PREV);
		addopt('n', XMPCURSES_MOD_NEXT);
		addopt('s', 7);
		addopt('m', 8);
		addpopt('u', 9);
		default:
			return !!write(1, help, 192);
			break;
	}
	if(out == 0) return !!write(1, help, 192);
	int fd = shm_open("/xmp-curses.shm", O_RDWR, 0777);
	if(fd < 0) return !!write(1, "could not open shared memory\n", 29);
	char* mem = mmap(NULL, 1, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	if(out == 9) {
		char* respath = realpath(packet, NULL);
		if(!respath) return !!write(1, "bad path\n", 9);
		munmap(mem, 1);
		ftruncate(fd, 4+strlen(respath));
		mem = mmap(NULL, 4+strlen(respath), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
		memcpy(mem + 1, respath, strlen(respath)+1);
		free(respath);
	}
	mem[0] = out;
	while(mem[0] != 0) {}
	if(out == 7 || out == 8) {
		munmap(mem, 1);
		fstat(fd, &s);
		mem = mmap(NULL, s.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
		printf("%s\n", mem + 1);
	}
}
