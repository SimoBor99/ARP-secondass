#include "./../include/processB_utilities.h"

// we initialize stuff...
int posx = 0;
int posy = 0;
int width = 1600;
int height = 600;
int depth = 4;
const int SIZE = 960000;
int i, fd_shm;
char * ptr;

int main(int argc, char const *argv[])
{
	    const char * shm_name = argv[1];
    	// Utility variable to avoid trigger resize event on launch
    	int first_resize = TRUE;

    	// Initialize UI
    	init_console_ui();

	    mvaddch(LINES/2, COLS/2, '0');
        refresh();

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
        	}

        	else {
            
        	}
    	}

    	endwin();
    	return 0;
}
