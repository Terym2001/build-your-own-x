#ifndef TSH_H_
#define TSH_H_

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void tsh_loop(void);
char* tsh_read_line(void);
char** tsh_split_line(char* line);
int tsh_launch(char** args);
int tsh_execute(char** args);

// Function declarations for builtin shell commands
int tsh_cd(char** args);
int tsh_help(char** args);
int tsh_exit(char** args);

// List of builtin commands, followed by their corresponding function
extern char* builtin_str[];

extern int (*builtin_func[]) (char**);

int tsh_num_builtins();
#endif
