#ifndef __TYPES_H
#define __TYPES_H

#include "includes.h"
#include "param.h"

typedef struct __info {
  char str[CMD_SIZE];
  int val;
} info_t;

typedef struct __node {
  info_t inf;
  struct __node *prev;
  struct __node *next;
} node_t;

typedef struct __list {
  node_t *head;
} list_t;

typedef struct __pastevents {
  char events[PAST_SIZE][CMD_SIZE];
  char path[PATH_MAX];
  int head;
} pastevents_t;

typedef struct __metadata {
  char home[PATH_MAX];
  char oldpwd[PATH_MAX];
  char last[CMD_SIZE];
  float extime;
  info_t running;
  pastevents_t past;
  list_t proc;
  jmp_buf mark;
} metadata_t;

typedef struct __interface {
  char uname[LOGIN_NAME_MAX];
  char hname[HOST_NAME_MAX];
  char cdir[PATH_MAX];
  char rpath[PATH_MAX];
} interface_t;

typedef struct __seekargs {
  char fname[PATH_MAX];
  char dname[PATH_MAX];
  int d_flag;
  int e_flag;
  int f_flag;
} seekargs_t;

#endif
