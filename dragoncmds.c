#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
pid_t childpid = -1;

#define MAX_ARGS 5  // Max number of arguments to a command
void dragon_clean_exit() {
  if (childpid > 0) {
    kill(childpid, SIGTERM);
  }
}

/**
 * Prints the current working directory
 */
void dragon_pwd(void) {
  char* buffer = getcwd(NULL, 0);

  if (buffer == NULL) {
    printf("failed to get current directory\n");
  } else {
    printf("%s \n", buffer);
  }
  free(buffer);
}
/**
 * changes the current working directory
 */
void dragon_cd(char* dir) {
  if (dir == NULL) {
    printf("no arguements provided\n");
    return;
  }
  if (chdir(dir) == -1) {
    printf("No such file or directory\n");
  }
}

/**
 * Spawn a new process for executing a new program based on the provided
 * arguements
 */
void external_cmds(char** argv) {
  char* input_file = NULL;
  char* output_file = NULL;
  int pipe_active = 0;
  char* pipe_argv[MAX_ARGS] = {NULL};
  int run_in_background = 0;
  // check for commands
  // IODIR SUPPORTED FOR EXTRNL PRGMS ONLY, YAY!
  // same with pipes

  // lazy check
  for (int i = 0; i < MAX_ARGS; i++) {
    if (argv[i] == NULL) {
      continue;
    } else if (!strcmp(argv[i], "&")) {
      run_in_background = 1;
      break;
    }
  }

  for (int i = 0; i < MAX_ARGS; i++) {
    if (argv[i] == NULL) {
      continue;
    }
    // iodir
    // i assume that the files dont have any arguements
    if (!strcmp(">", argv[i]) && argv[i + 1] != NULL) {
      output_file = argv[i + 1];
      argv[i] = NULL;
      argv[i + 1] = NULL;
    } else if (!strcmp("<", argv[i]) && argv[i + 1] != NULL) {
      input_file = argv[i + 1];
      argv[i] = NULL;
      argv[i + 1] = NULL;
    }
    // piping
    else if (!strcmp("|", argv[i]) && argv[i + 1] != NULL) {
      pipe_active = 1;
      int j = 0;
      argv[i] = NULL;
      // copy to pipe argv
      while (argv[i + 1] != NULL) {
        pipe_argv[j] = argv[i + 1];
        // wipe argv
        argv[i + 1] = NULL;
        i++;
        j++;
      }
      pipe_argv[j] = NULL;
    }
  }

  // spawn the child
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork failed");
  }
  // child process
  else if (pid == 0) {
    childpid = getpid();

    if (output_file != NULL) {
      int fid = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fid < 0) {
        printf("could not open output file\n");
        _exit(1);
      }
      dup2(fid, STDOUT_FILENO);
      close(fid);
    }
    if (input_file != NULL) {
      int fid = open(input_file, O_RDONLY);
      if (fid < 0) {
        printf("could not open input file\n");
        _exit(1);
      }
      dup2(fid, STDIN_FILENO);
      close(fid);
    }

    // start piping
    if (pipe_active) {
      int pipefd[2];
      pipe(pipefd);
      if (pipefd[1] < 0) {
        perror("pipe failed");
      }

      pid_t pipe_fork_id = fork();

      if (pipe_fork_id < 0) {
        perror("pipe fork failed");
      }
      // write side of pipe
      else if (pipe_fork_id == 0) {
        // close read side
        close(pipefd[0]);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);
        execve(argv[0], argv, NULL);
        _exit(1);
      }
      // read side of pipe
      else if (pipe_fork_id > 0) {
        // close write side
        close(pipefd[1]);
        // wait for write to finish
        wait(NULL);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execve(pipe_argv[0], pipe_argv, NULL);
        
      }
    }

    if (execve(argv[0], argv, NULL)) {
      ("Command not found \n");
    }
    _exit(1);
  }
  // parent should wait until the child is done
  // unless we run in background
  else if (pid > 0) {
    if (run_in_background == 0) {
      wait(NULL);
    }
  }
}