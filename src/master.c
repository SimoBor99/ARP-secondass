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
    if(execvp (program, arg_list) == 0);
    perror("Exec failed");
    return 1;
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
    exit(2);
  }
}

int main() {
  
  // define shared memory
  const char * shm_name="/AOS";
  void * ptr;
  int shm_fd;
  shm_fd=shm_open(shm_name, O_CREAT | O_RDWR, 0666);
  
  // check if the segment memory is valid
  if (shm_fd==1) {
    printf("SHared memory segment failed\n");
    exit(1);
  }
  
  // signal for kill other processes
  struct sigaction s_kill;
  memset(&s_kill, 0, sizeof(s_kill));
  s_kill.sa_handler=&kill_processes;

  // catch signal handler
  if (sigaction(SIGINT, &s_kill, 0)==-1) {
    perror("Can't catch the signal");
  }

  // define list of arguments of processes
  char * arg_list_A[] = { "/usr/bin/konsole", "-e", "./bin/processA", NULL };
  char * arg_list_B[] = { "/usr/bin/konsole", "-e", "./bin/processB", NULL };

  pid_t pid_procA = spawn("/usr/bin/konsole", arg_list_A);
  pid_t pid_procB = spawn("/usr/bin/konsole", arg_list_B);

  int status;
  if ( waitpid(pid_procA, &status, 0)==-1) {
    perror("Error in waitpid!");
    exit(2);
  }
  if (waitpid(pid_procB, &status, 0)==-1) {
    perror("Error in waitpid!");
    exit(3);
  }
  if (shm_unlink(shm_name)==1) {
    printf("Error removing %s\n", shm_name);
    exit(4);
  }
  printf ("Main program exiting with status %d\n", status);
  return 0;
}

