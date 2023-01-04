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
#include <string.h>
#include <semaphore.h>
#define SEM_PATH_1 "/sem_AOS_1"
#define SEM_PATH_2 "/sem_AOS_2"

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
    strcpy(bmp_vec, ptr);
	printf("%s", bmp_vec);
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
	if (fd_shm ==-1) {
		printf("Shared memory segment failed\n");
		exit(1);
	}

	// we map the shared memory
	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, fd_shm, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
	}
	sem_t * sem_id1=sem_open(SEM_PATH_1, 0);
	if ( sem_id1==SEM_FAILED) {
		perror("semaphore id1 cannot be opened");
		exit(EXIT_FAILURE);
	}
	sem_t * sem_id2=sem_open(SEM_PATH_2, 0);
	if ( sem_id2==SEM_FAILED) {
		perror("semaphore id2 cannot be opened");
		exit(EXIT_FAILURE);
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
					if (sem_wait(sem_id2)==-1) {
							perror("It is not possible execute wait");
							if (sem_close(sem_id2)==-1) {
								if (sem_unlink(SEM_PATH_2)==-1) {
									perror("Cannot unlink sem_id2");
									exit(EXIT_FAILURE);
								}
								exit(EXIT_FAILURE);
							}
							if (sem_unlink(SEM_PATH_2)==-1) {
								perror("Cannot unlink sem_id2");
								exit(EXIT_FAILURE);
							}
							exit(EXIT_FAILURE);
						}
            		read_shm();
						if (sem_post(sem_id1)==-1) {
							perror("It is not possible execute post");
							if (sem_close(sem_id1)==-1) {
								if (sem_unlink(SEM_PATH_1)==-1) {
									perror("Cannot unlink sem_id1");
									exit(EXIT_FAILURE);
								}
								exit(EXIT_FAILURE);
							}
							if (sem_unlink(SEM_PATH_1)==-1) {
								perror("Cannot unlink sem_id1");
								exit(EXIT_FAILURE);
							}
							exit(EXIT_FAILURE);
						}
        	}
    	}

    if (close(fd_shm)==-1) {
    	perror("Cannot close the the file descriptor");
  	}

	if (sem_close(sem_id1)==-1) {
		if (sem_unlink(SEM_PATH_1)==-1) {
				perror("Cannot unlink sem_id1");
				exit(EXIT_FAILURE);
		}
		exit(EXIT_FAILURE);
	}

	if (sem_close(sem_id2)==-1) {
			if (sem_unlink(SEM_PATH_2)==-1) {
				perror("Cannot unlink sem_id2");
				exit(EXIT_FAILURE);
			}
			exit(EXIT_FAILURE);
	}

	if (sem_unlink(SEM_PATH_1)==-1) {
				perror("Cannot unlink sem_id1");
				exit(EXIT_FAILURE);
	}

	if (sem_unlink(SEM_PATH_2)==-1) {
				perror("Cannot unlink sem_id2");
				exit(EXIT_FAILURE);
			}

  	//remove the mapping
  	if (munmap(ptr, sizeof(char))==-1) {
    	perror("Cannot unmapp the address");
    	exit(1);
  	}

  	if (shm_unlink(shm_name)==-1) {
    	printf("Error removing %s\n", shm_name);
    	exit(1);
  	}
	endwin();
    return 0;
}
