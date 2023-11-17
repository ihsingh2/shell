#include "../headers/defs.h"

void init_list(list_t *lst)
{
  lst->head = (node_t*) malloc(sizeof(node_t));

  lst->head->inf.val = -1;
  strcpy(lst->head->inf.str, "NIL");

  lst->head->prev = lst->head;
  lst->head->next = lst->head;
}

void insert(list_t *lst, char *str, int num)
{
  node_t *x = (node_t*) malloc(sizeof(node_t));
  x->inf.val = num;
  strcpy(x->inf.str, str);
  
  lst->head->prev->next = x;
  x->prev = lst->head->prev;
  lst->head->prev = x;
  x->next = lst->head;
}

void delete(list_t *lst, node_t *x)
{
  if (lst->head == x)
    return;
  
  x->prev->next = x->next;
  x->next->prev = x->prev;
    
  free(x);
}

node_t* search(list_t *lst, int x)
{
  node_t *p = lst->head;
  p = p->next;

  while (p != lst->head)
  {
    if (p->inf.val == x)
      return p;

    p = p->next;
  }

  return NULL;
}
