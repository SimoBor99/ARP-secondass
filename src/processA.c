// we include libraries
#include "./../include/processA_utilities.h"
#include <stdio.h>
#include <bmpfile.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>

// we initialize stuff...
int n_snapshot = 0;
int posx = 0;
int posy = 0;
int width = 1600;
int height = 600;
int depth = 4;
char path[100] = "out/snap";

// we declare bmp file, bmp_vec and pixel
bmpfile_t *bmp;
rgb_pixel_t pixel = {255, 0, 0, 0};
rgb_pixel_t * pixelv;
int bmp_vec[960000];

// function to be executed when the 'P' button is pressed
void print_circle(int px, int py) {
	px *= 20;
	py *= 20;
	bmpfile_t *bmpp;
  	rgb_pixel_t pixelp = {255, 0, 0, 0};
  	rgb_pixel_t pixelw = {0, 0, 0, 0};
  
  	char num[20];
	bmpp = bmp_create(width, height, depth);
  
	int radius = 30;
	for(int x = -radius; x <= radius; x++) {
		for(int y = -radius; y <= radius; y++) {
	      		if(sqrt(x*x + y*y) < radius) {
          			bmp_set_pixel(bmpp, width/2 + x + px, height/2 + y - py, pixelp);
      			}
			else {
      				bmp_set_pixel(bmp, width/2 + x, height/2 + y, pixelw);
      			}
    		}
  	}
  	sprintf(num,"%d",n_snapshot);
  	strcat(path,num);
  	strcat(path,".bmp");	// TODO: Chiedere su creazione cartella out
  	bmp_save(bmpp, path);	// TODO: Chiedere a Macciò se va bene rimpiazzare gli snaphot in esecuzioni diverse
  	n_snapshot += 1;
  	bmp_destroy(bmpp);
}

// function to transform the bmp file into a vector to be loaded into the shared memory
void vectorize() {
	int val;
	int count = 0;
	for(int i = 0; i < width; i++) {
		for(int j = 0; j < height; j++) {
			pixelv = bmp_get_pixel(bmp,i,j);
			val = pixelv->blue;
			if (val == 255) {
				bmp_vec[count]=val;
			}
			else {
				bmp_vec[count]=val;
			}
			count += 1;
		}
	}
		
}

// function to initialize the bmp file
void init_bmp() {
  	rgb_pixel_t pixelb = {255, 0, 0, 0};
  	rgb_pixel_t pixelw = {0, 0, 0, 0};
  	int countz = 0;
  
  	char num[20];
	bmp = bmp_create(width, height, depth);
  
	int radius = 30;
	
	for(int a = 0; a < width; a++) {
		for (int b = 0; b < height; b++) {
			bmp_set_pixel(bmp, a, b, pixelw);
		}
	}
	
	for(int x = -radius; x <= radius; x++) {
    		for(int y = -radius; y <= radius; y++) {
      		// If distance is smaller, point is within the circle
      			if(sqrt(x*x + y*y) < radius) {
          			bmp_set_pixel(bmp, width/2 + x, height/2 + y, pixelb);
      			}
    		}
  	}
}

void write_shm (int fd_shm) {
	int *ptr=mmap(0, sizeof(bmp_vec), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
	if (ptr==MAP_FAILED) {
		printf("Map failed\n");
		exit(1);
	}
	// write into memory segment
	for ( int i=0; i<960000; i++) {
		ptr[i]=bmp_vec[i];
		ptr=ptr+1;
	}
	munmap(ptr, sizeof(bmp_vec));
	
}
// function to modify the bmp file if the circle has been moved
void move_bmp(int posx, int posy) {
}

// main function
int main(int argc, char *argv[])
{
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;
 
    // shared memory fd
    int fd_shm=atoi(argv[1]);
    // Initialize UI
    init_console_ui();
    
    // we initialize the bmp file of process A
    init_bmp();
    vectorize();
    ftruncate(fd_shm, sizeof(bmp_vec));
    
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
                    print_circle(posx,posy);
                    sleep(1);
                    for(int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
        }

        // If input is an arrow key, move circle accordingly...
        else if(cmd == KEY_LEFT || cmd == KEY_RIGHT || cmd == KEY_UP || cmd == KEY_DOWN) {
            
            switch(cmd) {
            
            	case KEY_LEFT:
            		if (posx > -40) {
            			posx -= 1;
            			move_circle(cmd);
            			draw_circle();
            			move_bmp(posx, posy);
            			vectorize();
            			//write_shm(fd_shm);
            			usleep(20000);
            		}
            		break;
            	case KEY_RIGHT:
            		if (posx < 40) {
            			posx += 1;
            			move_circle(cmd);
            			draw_circle();
            			move_bmp(posx, posy);
            			vectorize();
            			//write_shm(fd_shm);
            			usleep(20000);
            		}
            		break;
            	case KEY_DOWN:
            		if (posy > -15) {
            			posy -= 1;
            			move_circle(cmd);
            			draw_circle();
            			move_bmp(posx, posy);
            			vectorize();
            			//write_shm(fd_shm);
            			usleep(20000);
            		}
            		break;
            	case KEY_UP:
            		if (posy < 15) {
            			posy += 1;
            			move_circle(cmd);
            			draw_circle();
            			move_bmp(posx, posy);
            			vectorize();
            			//write_shm(fd_shm);
            			usleep(20000);
            		}
            		break;
            	default:
            		usleep(20000);
            		break;
            }
        }
        
        // we load the data into the shared memory
        
        usleep(20000);
    }
    
    endwin();
    return 0;
}
