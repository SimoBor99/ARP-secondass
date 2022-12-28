// we include libraries
#include "./../include/processA_utilities.h"
#include <stdio.h>
#include <bmpfile.h>
#include <stdlib.h>
#include <math.h>

int n_snapshot = 0;

// function to be executed when the 'P' button is pressed
void print_circle() {
	bmpfile_t *bmp;
  	rgb_pixel_t pixel = {255, 0, 0, 0};
  
  	int width = 100;
  	int height = width;
  	int depth = 4;
  	char path[100] = "out/snap";
  	char num[20];
	bmp = bmp_create(width, height, depth);
  
	int radius = 30;
	for(int x = -radius; x <= radius; x++) {
		for(int y = -radius; y <= radius; y++) {
	      		if(sqrt(x*x + y*y) < radius) {
          			bmp_set_pixel(bmp, width/2 + x, height/2 + y, pixel);
      			}
    		}
  	}
  	sprintf(num,"%d",n_snapshot);
  	strcat(path,num);
  	strcat(path,".bmp");
  	bmp_save(bmp, path);
  	n_snapshot += 1;
  	bmp_destroy(bmp);
}

// main function
int main(int argc, char *argv[])
{
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize UI
    init_console_ui();

    // Infinite loop
    while (TRUE)
    {
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

        // Else, if user presses print button...
        else if(cmd == KEY_MOUSE) {
            if(getmouse(&event) == OK) {
                if(check_button_pressed(print_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Print button pressed");
                    refresh();
                    print_circle();
                    sleep(1);
                    for(int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
        }

        // If input is an arrow key, move circle accordingly...
        else if(cmd == KEY_LEFT || cmd == KEY_RIGHT || cmd == KEY_UP || cmd == KEY_DOWN) {
            move_circle(cmd);
            draw_circle();
        }
    }
    
    endwin();
    return 0;
}
