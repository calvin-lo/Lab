/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef MYSHELL_H_
#define MYSHELL_H_

// Include your relevant functions declarations here they must start with the 
// extern keyword such as in the following example:
// extern void display_help(void);
extern int lshloop(void);
extern char *lsh_read_line(void);
extern char **lsh_split_line(char *line);
extern int lsh_launch(char **args);

extern int lsh_cd(char **args);
extern int lsh_clr();
extern int lsh_dir();
extern int lsh_environ();
extern int lsh_echo();
extern int lsh_help();
extern int lsh_pause();
extern int lsh_quit();

extern int lsh_num_builtins();
extern int lsh_execute(char **args);

#endif /* MYSHELL_H_ */