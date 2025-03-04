#include <stdlib.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <time.h>
#include<unistd.h>
#include <signal.h>
#include <errno.h>
#include<string.h>
#include<fcntl.h>

#include "dragoncmds.h"


#define LINE_LENGTH 100 // Max # of characters in an input line
#define MAX_ARGS 5 // Max number of arguments to a command
#define MAX_LENGTH 20 // Max # of characters in an argument
#define MAX_BG_PROC 1 // Max # of processes running in the background

pid_t shell_pid = -1;
/**
 * @brief Tokenize a C string 
 * 
 * @param str - The C string to tokenize 
 * @param delim - The C string containing delimiter character(s) 
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
void tokenize(char* str, const char* delim, char ** argv){
  char* token;
  token = strtok(str, delim);
  for(size_t i = 0; token != NULL; ++i){
    argv[i] = token;
  token = strtok(NULL, delim);
  }
}

void sig_handle_int(int sig){
  pid_t pid = getpid();
  if (pid != shell_pid){
    kill(pid, SIGINT);
    write(STDOUT_FILENO, "\n", 1);
  }
  else if(pid == shell_pid){
    write(STDOUT_FILENO, "\n", 1);
  }
}
void sig_handle_stp(int sig){
  pid_t pid = getpid();
  if (pid != shell_pid){
    kill(pid, SIGTSTP);
    write(STDOUT_FILENO, "\n", 1);
  }
  else if(pid == shell_pid){
    write(STDOUT_FILENO, "\n", 1);
  }
}
void sig_handle_chld(int sig){
  int status;
  while (waitpid(-1, &status, WNOHANG) > 0);
}

void init_sig_handlers(){
  shell_pid = getpid();
  struct sigaction sig_int;
  sig_int.sa_handler = sig_handle_int;
  sig_int.sa_flags = 0;
  sigemptyset(&sig_int.sa_mask);
  sigaction(SIGINT, &sig_int, NULL);

  struct sigaction sig_stp;
  sig_stp.sa_handler = sig_handle_stp;
  sig_stp.sa_flags = 0;
  sigemptyset(&sig_stp.sa_mask);
  sigaction(SIGTSTP, &sig_stp, NULL);

  struct sigaction sig_chld;
  sig_chld.sa_handler = sig_handle_chld;
  sigemptyset(&sig_chld.sa_mask);
  sig_chld.sa_flags = 0; 
  sigaction(SIGCHLD, &sig_chld, NULL);
}

void arg_handler(char** argv){
  char* input_file = NULL;
  char* output_file = NULL;
  if(argv[0] == NULL){
    return;
  }

  if(!strcmp(argv[0],"pwd"))
    dragon_pwd();

  else if(!strcmp(argv[0],"cd"))
    dragon_cd(argv[1]);

  // not a built in command, shell will try to execute an external program
  // in a new process
  else
    external_cmds(argv);
}


void shell_loop(void){
  char* buffer = (char*) malloc(LINE_LENGTH * sizeof(char));
  // max args 5 doubling assuming pipes can have 5 args
  char** argv = (char**) malloc(MAX_ARGS * 2 * sizeof(char *));

  while(1){
    // empty out argv
    for (int i = 0; i < MAX_ARGS * 2; i++){
      argv[i] = NULL;
    }
    printf("dragonshell > ");

    if (fgets(buffer, LINE_LENGTH *sizeof(char), stdin) == NULL) {
      if (errno == EINTR) {
                // Interrupted by signal, print prompt again
                continue;
            }
    }
    buffer[strlen(buffer) - 1] = 0;

    tokenize(buffer, " ", argv);
    if (!strcmp(argv[0],"exit")){
      break;
    }
    arg_handler(argv);
    
  }
  free(argv);
  free(buffer);
}
  

int main(int argc, char *argv[]){
  init_sig_handlers();
  struct tms t_buf;
  times(&t_buf);
  long tps = sysconf(_SC_CLK_TCK);

  system("clear");
  printf("                                             __----~~~~~~~~~~~------___\n");
  printf("                                  .  .   ~~//====......          __--~ ~~\n");
  printf("                  -.            \\_|//     |||\\\\  ~~~~~~::::... /~\n");
  printf("               ___-==_       _-~o~  \\/    |||  \\\\            _/~~-\n");
  printf("       __---~~~.==~||\\=_    -_--~/_-~|-   |\\\\   \\\\        _/~              ASCII ART FROM\n");
  printf("   _-~~     .=~    |  \\\\-_    '-~7  /-   /  ||    \\      /                 www.asciiart.eu/mythology/dragons\n");
  printf(" .~       .~       |   \\\\ -_    /  /-   /   ||      \\   /                   ________                                      \n");
  printf("/  ____  /         |     \\\\ ~-_/  /|- _/   .||       \\ /                    \\______ \\____________     ____   ____   ____   \n");
  printf("|~~    ~~|--~~~~--_ \\     ~==-/   | \\~--===~~        .\\                      |    |  \\_  __ \\__  \\   / ___\\ /  _ \\ /    \\  \n");
  printf("         '         ~-|      /|    |-~\\~~       __--~~                        |    `   \\  | \\// __ \\_/ /_/  >  <_> )   |  \\ \n");
  printf("                     |-~~-_/ |    |   ~\\_   _-~            /\\               /_______  /__|  (____  /\\___  / \\____/|___|  / \n");
  printf("                          /  \\     \\__   \\/~                \\__                     \\/           \\//_____/             \\/  \n");
  printf("                      _--~ _/ | .-~~____--~-/                 ~~==.             _________.__           .__  .__             \n");
  printf("                     ((->/~   '.|||'-_|    ~~-/ ,              . _||           /   _____/|  |__   ____ |  | |  |            \n");
  printf("                                -_     ~\\      ~~---l__i__i__i--~~_/           \\_____  \\ |  |  \\_/ __ \\|  | |  |             \n");
  printf("                                _-~-__   ~)  \\--______________--~~             /        \\|   Y  \\  ___/|  |_|  |__          \n");
  printf("                              //.-~~~-~_--~- |-------~~~~~~~~                 /_______  /|___|  /\\___  >____/____/         \n");
  printf("                                     //.-~~~--\\                                       \\/      \\/     \\/                     \n");
  printf("--------------------------------------------------------------------------------------------------------------------------\n");
  
  init_sig_handlers();
  shell_loop();

  times(&t_buf);
  printf("User time: %li\n", (int) t_buf.tms_cutime / tps);
	printf("System time: %li\n", (int) t_buf.tms_cstime / tps);
  dragon_clean_exit();
  

  return 0;
}


