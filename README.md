# DragonShell

A simple and easy to use Linux Shell

## Design Choices

- **Command Handling**: Commands are basically just done by tokenizing an arg string. The built-in commands (`pwd`, `cd`, `exit`) are handled in the `arg_handler` function, while external commands are managed in `external_cmds`.

- **Input/Output Redirection**: Copy the read and write files into their own strings. If set then open the respective files for redirection.

- **Piping**: Spilt the commands into 2 argv variables On process is forked and parent handles the other. Parent get results from child.

- **Signal Handling**: Basically just created handlers that send the signals to the processes that are running and not the shell. sigchld is for preventing zombies.

- **Background**: Just a flag. If set we will just not tell the shell to wait for the process to finish.

## System Calls Used

The following system calls were used:

- **`fork()`**: Used for running external programs in new processes.
- **`execve()`**: Used to execute external programs.
- **`waitpid() & wait()`**: Used to wait for child processes to finish.
- **`kill()`**: exiting the shell, sending signals for signal handler.
- **`pipe()`**: piping.
- **`chdir()`**: Used to change the current working directory.
- **`getcwd()`**: Used to get the current working directory.
- **`open()`**: Used to open files for input/output redirection.
- **`dup2()`**: Used to duplicate file descriptors for redirection.

## Testing

The shell was tested through various command scenarios to ensure all features were functioning as intended. The testing process included:

1. **Basic Command Execution**: Tested built-in commands (`pwd`, `cd`, `exit`) to verify their functionality. Afterwards external commands were tested (ls, cat, find, as well as my own custom programs)
2. **Input/Output Redirection**: Followed the testing with a.out reference in the assignment doc. Did output redir to create a.out and then tested input redir with cat
4. **Piping**: Followed the find sort example in the assignment doc
5. **Signal Handling**: Tested with wait.c which provided from lab 1
3. **Background Processes**: Launched processes with `&` to verify they could run independently without blocking the shell.

## Sources

- [Die.net](https://www.die.net/)



