#ifndef __DEFS_H
#define __DEFS_H

#include "includes.h"
#include "types.h"
#include "param.h"
#include "colors.h"

// activities.c
void handle_activities(metadata_t *vars, char **argv);
void print_process_info(char *cmd, int pid);

// bg.c
void handle_bg(metadata_t *vars, char **argv);

// exec.c
void exec_bg_job(metadata_t *vars, char *job);
void exec_fg_job(metadata_t *vars, char *job);
void exec_piped_bg_job(metadata_t *vars, char *job);
void exec_piped_fg_job(metadata_t *vars, char *job);

// fg.c
void handle_fg(metadata_t *vars, char **argv);
void execute_fg(metadata_t *vars, int pid);

// iman.c
void handle_iman(metadata_t *vars, char **argv);
void execute_iman(metadata_t *vars, char *name);

// input.c
int check_input(char *input, char *ret);
void handle_input(metadata_t *vars, char *input);
void handle_command(metadata_t *vars, char *command);

// jobs.c
void handle_background_jobs(metadata_t *vars, char *job);
void handle_foreground_jobs(metadata_t *vars, char *job);
void check_background_jobs(metadata_t *vars);

// list.c
void init_list(list_t *lst);
void insert(list_t *lst, char *str, int num);
void delete(list_t *lst, node_t *x);
node_t* search(list_t *lst, int x);

// main.c
int main(void);
void init_env(void);
void refresh_env(void);
void int_handler(int sig);
void tstp_handler(int sig);

// neonate.c
void handle_neonate(metadata_t *vars, char **argv);
void execute_neonate(metadata_t *vars, int time);
void print_recent_process(int time);
void die(const char *s);
void disableRawMode();
void enableRawMode();

// pastevents.c
void init_pastevents(metadata_t *vars);
void add_pastevent(metadata_t *vars, char* arg);
int process_pastevent(metadata_t *vars, char* arg);
void sync_pastevents(metadata_t *vars);
void handle_pastevents(metadata_t *vars, char **argv);
void display_pastevents(metadata_t *vars);
void purge_pastevents(metadata_t *vars);

// peek.c
void handle_peek(metadata_t *vars, char **argv);
void execute_peek(metadata_t *vars, char *dname, int a_flag);
void execute_peek_longlist(metadata_t *vars, char *dname, int a_flag);

// ping.c
void handle_ping(metadata_t *vars, char **argv);

// pipe.c
void exec_piped_bg_job(metadata_t *vars, char *job);
void exec_piped_fg_job(metadata_t *vars, char *job);

// proclore.c
void handle_proclore(metadata_t *vars, char **argv);
void execute_proclore(metadata_t *vars, int arg);

// prompt.c
void prompt(metadata_t *vars);
void refresh_intf(metadata_t *vars, interface_t *intf);

// seek.c
void handle_seek(metadata_t *vars, char **argv);
void execute_seek(metadata_t *vars, seekargs_t *args);
void search_directory(metadata_t *vars, seekargs_t *args, int *found, char *path, char *cdir);

// utilities.c
int is_empty(char *str);
int count_occurrences(char *str, char target);
void tokenize(char **argv, char *str, char *delim);
void tokenize2d(char *argv[][CMD_SIZE], char *str, char *delim1, char *delim2);
int handle_redirection(metadata_t *vars, char **argv, int *in, int *out);
void addtilde(char *dest, char* src, char* home);
void remtilde(char *dest, char* src, char* home);
int modulo(int a, int m);
int nmodulo(int a, int m);
int is_executable(char *dirname, char *path);
void get_permissions(char *perms, mode_t mode);

// warp.c
void handle_warps(metadata_t *vars, char **argv);
void handle_warp(metadata_t *vars, char *arg);
void execute_warp(metadata_t *vars, char *arg);

#endif
