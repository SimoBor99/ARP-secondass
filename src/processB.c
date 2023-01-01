#include "./../include/processB_utilities.h"
#include <stdio.h>
#include <bmpfile.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>

// we initialize stuff...
int posx = 0;
int posy = 0;
int width = 1600;
int height = 600;
int depth = 4;
const int SIZE = 960000;
int i, fd_shm;
char * ptr;

// we declare bmp file, bmp_vec and pixel
bmpfile_t *bmp;
rgb_pixel_t pixelb = {255, 0, 0, 0};
rgb_pixel_t pixelw = {0, 0, 0, 0};
char bmp_vec[960000];

// function for reading from shared memory
void read_shm() {
	int countn = 0;
	sprintf(bmp_vec,"%s", (char *) ptr);
	//printf("%s", bmp_vec);
	for(int i = 0; i < SIZE; i++) {
	if (bmp_vec[i] == 48) {
		printf("%d\n", countn);
		countn++;
	}
	}
	printf("%d\n", countn);
	
}

// main function
int main(int argc, char const *argv[])
{
	const char * shm_name = argv[1];
    	// Utility variable to avoid trigger resize event on launch
    	int first_resize = TRUE;

    	// Initialize UI
    	init_console_ui();

	mvaddch(LINES/2, COLS/2, '0');
        refresh();
        posx = COLS/2;
        posy = LINES/2;

	// we open the shared memory
	fd_shm = shm_open(shm_name, O_RDONLY, 0666);
	if (fd_shm == 1) {
		printf("Shared memory segment failed\n");
		exit(1);
	}

	// we map the shared memory
	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, fd_shm, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
	}
	
    	// Infinite loop
    	while (TRUE) {

        	// Get input in non-blocking mode
        	int cmd = getch();

        	// If user resizes screen, re-draw UI...
        	if(cmd == KEY_RESIZE) {
            		if(first_resize) {
                		first_resize = FALSE;
            		}
            		else {
                		reset_console_ui();
            		}
            		mvaddch(LINES/2, COLS/2, '0');	// TO CANCEL AFTER DEBUGGING
        		refresh();			// TO CANCEL AFTER DEBUGGING
        	}

        	else {
            		read_shm();
        	}
    	}

    	endwin();
    	return 0;
}
