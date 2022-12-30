#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <signal.h>

// declaration pids of two proccesses
pid_t pid_procA, pid_procB;

// size of shared memory
const int dim_mem=1600*600;

int spawn(const char * program, char * arg_list[]) {

  pid_t child_pid = fork();

  if(child_pid < 0) {
    perror("Error while forking...");
    return 1;
  }

  else if(child_pid != 0) {
    return child_pid;
  }

  else {
    if(execvp (program, arg_list) == 0) {
    perror("Exec failed");
    return 1;
    }
  }
}

// kill processes handler when user presses ctrl+c
void kill_processes ( int signum) {
 
  // send signal to process A
  if (kill(pid_procA, SIGTERM)==-1) {
    perror("Something went wrong in kill");
    exit(1);
  }

  // send signal to process B
  if (kill(pid_procB, SIGTERM)==-1) {
    perror("Something went wrong in kill");
    exit(1);
  }
}

int main() {
  
  // define shared memory
  char * shm_name="/AOS";
  int shm_fd;
  shm_fd=shm_open(shm_name, O_CREAT | O_RDWR, 0666);
  
  // check if the segment memory is valid
  if (shm_fd==-1) {
    printf("Shared memory segment failed\n");
    exit(1);
  }
  if (ftruncate(shm_fd, dim_mem)==-1) {
    perror("Cannot truncate!");
    return shm_fd;
  }
  
  // map the address process
  char *ptr=mmap(0, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

  // check if the mapping has done
  if (ptr == MAP_FAILED) {
    printf("Map failed\n");
    exit(1);
  }

  // signal for kill other processes
  struct sigaction s_kill;
  memset(&s_kill, 0, sizeof(s_kill));
  s_kill.sa_handler=&kill_processes;
  s_kill.sa_flags=SA_RESTART;

  // catch signal handler
  if (sigaction(SIGINT, &s_kill, 0)==-1) {
    perror("Can't catch the signal");
  }

  // define list of arguments of processes
  char * arg_list_A[] = { "/usr/bin/konsole", "-e", "./bin/processA", shm_name, NULL };
  char * arg_list_B[] = { "/usr/bin/konsole", "-e", "./bin/processB", shm_name, NULL };

  pid_procA = spawn("/usr/bin/konsole", arg_list_A);
  pid_procB = spawn("/usr/bin/konsole", arg_list_B);

  int status;
  if ( waitpid(pid_procA, &status, 0)==-1) {
    perror("Error in waitpid 1!");
    return -1;
  }
  if (waitpid(pid_procB, &status, 0)==-1) {
    perror("Error in waitpid 2!");
    return -1;
  }

  // close the file descriptor
  if (close(shm_fd)==-1) {
    perror("Cannot close the the file descriptor");
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
  printf ("Main program exiting with status %d\n", status);
  return 0;
}

