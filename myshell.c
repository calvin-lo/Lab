/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include "utility.h"
#include "myshell.h"
 #include <sys/wait.h>

// Put macros or constants here using #define
#define BUFFER_LEN 256
#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

// Put global environment variables here
 extern char **environ;
// Define functions declared in myshell.h here
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      system(*args);
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    system(*args);
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}
char *builtin_str[] = {
  "cd","clr","dir","environ","echo","help","pause","quit"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd, &lsh_clr, &lsh_dir, &lsh_environ, &lsh_echo,
  &lsh_help, &lsh_pause, &lsh_quit
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

extern int lsh_clr()
{
    system("clear");
    return 1;
}
extern int lsh_dir(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    }
    else {
        DIR *dp;
        struct dirent *dir = NULL;
        dp = opendir(args[1]);
        if (dp)
        {
            while ((dir = readdir(dp)) != NULL)
                printf("%s\n", dir->d_name);
            closedir (dp);
        }
        else
            perror ("");
    }
    return 1;
}
extern int lsh_environ()
{
    int i = 0;
    while (environ[i])
        printf("%s\n", environ[i++]);

    return 1;
}
extern int lsh_echo(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        int i = 1;
        while (args[i] != NULL)  {
            printf("%s ", args[i]);
            i++;
        }
        printf("\n");
    }
    return 1;
}
extern int lsh_pause()
{
    char buffer[BUFFER_LEN] = { 0 };
    fgets(buffer,BUFFER_LEN,stdin); 

    return 1;
}

int lsh_help()
{
    char buffer[BUFFER_LEN] = { 0 };

    int i = 0;
    printf("Help: \n");
    char *line = malloc(BUFFER_LEN * sizeof(char*));
    FILE *fp = fopen("readme","r");
    while (fp)  {
        if (fgets(line,BUFFER_LEN,fp) == NULL) break;
        i++;
        printf("%s", line);
        if (i % 4 == 0)
            fgets(buffer,BUFFER_LEN,stdin); 
    }

    printf("\n");

  return 1;
}

int lsh_quit()
{
    return EXIT_SUCCESS;
}

int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

int main(int argc, char *argv[])
{
    // Input buffer and and commands
    char buffer[BUFFER_LEN] = { 0 };
    char command[BUFFER_LEN] = { 0 };
    char arg[BUFFER_LEN] = { 0 };
    char **tokens = malloc(BUFFER_LEN * sizeof(char*));
    char f[BUFFER_LEN] = {0};

    // Parse the commands provided using argc and argv
    if (argc == 2)
        strcpy(f, argv[1]);

    char *line = malloc(BUFFER_LEN * sizeof(char*));
    FILE *fp = fopen(f,"r");
    while (fp)  {
        if (fgets(line,BUFFER_LEN,fp) == NULL) break;
        //printf("%s", line);
        tokens = lsh_split_line(line);
        lsh_execute(tokens);

    }
    // Perform an infinite loop getting command input from users
    char cwd[BUFFER_LEN];
    printf("> %s$ ", getcwd(cwd, sizeof(cwd)));
    while (fgets(buffer, BUFFER_LEN, stdin) != NULL)
    {
        // Perform string tokenization to get the command and argument
        tokens = lsh_split_line(buffer);
        strcpy(command,tokens[0]);
        if (tokens[1] != NULL)
            strcpy(arg,tokens[1]);
        // Check the command and execute the operations for each command
        // cd command -- change the current directory
        if (strcmp(command, "cd") == 0)
        {
            // your code here
            lsh_cd(tokens);
            printf("> %s$ ", getcwd(cwd, sizeof(cwd)));
        }
        else if (strcmp(command, "clr") == 0)
        {
            lsh_clr();
            printf("> %s$ ", getcwd(cwd, sizeof(cwd)));
        }
        // dir
        else if (strcmp(command,"dir") == 0)
        {
            lsh_dir(tokens);
            printf("> %s$ ", getcwd(cwd, sizeof(cwd)));
        }
        // environ
        else if (strcmp(command,"environ") == 0)
        {
            lsh_environ();
            printf("> %s$ ", getcwd(cwd, sizeof(cwd)));
        }
        // echo
        else if (strcmp(command,"echo") == 0)
        {
            lsh_echo(tokens);
            printf("> %s$ ", getcwd(cwd, sizeof(cwd)));
        }
        // help
        else if (strcmp(command, "help") == 0)
        {
            lsh_help();
            printf("> %s$ ", getcwd(cwd, sizeof(cwd)));
        }
        // pause
        else if (strcmp(command,"pause") == 0)
        {
            lsh_pause();
            printf("> %s$ ", getcwd(cwd, sizeof(cwd)));
        }
        // quit command -- exit the shell
        else if (strcmp(command, "quit") == 0)
        {
            return EXIT_SUCCESS;
        }

        // Unsupported command
        else
        {
            lsh_launch(tokens);
            printf("> %s$ ", getcwd(cwd, sizeof(cwd)));   
            //fputs("Unsupported command, use help to display the manual\n", stderr);
        }
    }
    return EXIT_SUCCESS;
}
