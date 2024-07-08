#include "tsh.h"

char* builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char**) = {
  &tsh_cd,
  &tsh_help,
  &tsh_exit
};

#define BUF_SIZE 1024
void tsh_loop(void)
{
  char wd[BUF_SIZE];
  char* line;
  char** args;
  int status;

  do {
    // Get working directory
    if (getcwd(wd, sizeof(wd)) == NULL)
    {
      perror("getcwd() error");
      exit(EXIT_FAILURE);
    }
    // Print terminal prompt
    printf("%s> ", wd);

    line = tsh_read_line();
    args = tsh_split_line(line);
    status = tsh_execute(args);

    free(line);
    free(args);
  } while(status);
}

#define TSH_RL_BUFSIZE 1024
char* tsh_read_line(void)
{
  int bufsize = TSH_RL_BUFSIZE;
  int position = 0;
  char* buffer = malloc(sizeof(char) * bufsize);
  int c;

  if(!buffer)
  {
    fprintf(stderr, "tsh: allocation error, error: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  while(1)
  {
    // Read character
    c = getchar();

    // If we hit the EOF, replace with a null character and return
    if(c == EOF || c == '\n')
    {
      buffer[position] = '\0';
      return buffer;
    }
    else
    {
      buffer[position] = c;
    }
    position++;
    
    // If we have excedded the buffer, reealocate
    if(position >= bufsize)
    {
      bufsize += TSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize * sizeof(char));
      if(!buffer)
      {
        fprintf(stderr, "tsh: allocation error, error: %s", strerror(errno));
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define TSH_TOK_BUFSIZE 64
#define TSH_TOK_DELIM " \t\r\n\a"
char** tsh_split_line(char* line)
{
  int bufsize = TSH_TOK_BUFSIZE, position = 0; 
  char** tokens = malloc(bufsize * sizeof(char*));
  char* token;

  if(!tokens)
  {
    fprintf(stderr, "tsh: allocation error, error: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  token = strtok(line, TSH_TOK_DELIM);
  while(token != NULL)
  {
    tokens[position] = token;
    position++;

    if (position >= bufsize)
    {
      bufsize += TSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if(!tokens)
      {
        fprintf(stderr, "tsh: allocation error, error: %s", strerror(errno));
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, TSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int tsh_execute(char** args)
{
  if (args[0] == NULL)
  {
    // Empty command was entered
    return 1;
  }

  for (int i = 0; i < tsh_num_builtins(); i++)
  {
    if (strcmp(args[0], builtin_str[i]) == 0)
    {
      return (*builtin_func[i])(args);
    }
  }

  return tsh_launch(args);
}

int tsh_launch(char** args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if(pid == 0)
  {
    // Child process
    if(execvp(args[0], args) == -1)
    {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  }
  else if(pid < 0)
  {
    perror("lsh");
  }
  else
  {
    // Parent process
    do
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

// Builtin function implementations
int tsh_cd(char** args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "tsh: expected argument to \"cd\"\n");
  }
  else
  {
    if (chdir(args[1]) != 0)
    {
      perror("tsh");
    }
  }
  return 1;
}

int tsh_help(char** args)
{
  int i;
  printf("Trym Dyrkorn's TSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < tsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int tsh_exit(char** args)
{
  return 0;
}

int tsh_num_builtins()
{
  return sizeof(builtin_str) / sizeof(char*);
}
