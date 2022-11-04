//  Partie implantation du module holdall.

#include <stdlib.h>
#include <stdio.h>

#include "holdall.h"
#include "bunch.h"                                                    /* <--- */

//  struct holdall, holdall : implantation par liste dynamique simplement
//    chainée. L'insertion a lieu en queue si la macroconstante
//    HOLDALL_INSERT_TAIL est définie, en tête sinon.

typedef struct choldall choldall;

struct choldall {
  void *value;
  choldall *next;
};

struct holdall {
  choldall *head;
#ifdef HOLDALL_INSERT_TAIL
  choldall *tail;
#endif
  size_t count;
};

holdall *holdall_empty(void) {
  holdall *ha = malloc(sizeof *ha);
  if (ha == NULL) {
    return NULL;
  }
  ha->head = NULL;
#ifdef HOLDALL_INSERT_TAIL
  ha->tail = NULL;
#endif
  ha->count = 0;
  BUNCH_ADD(ha);                                                      /* <--- */
  return ha;
}

int holdall_put(holdall *ha, void *ptr) {
  BUNCH_ENSURE(ha);                                                   /* <--- */
  choldall *p = malloc(sizeof *p);
  if (p == NULL) {
    return -1;
  }
  p->value = ptr;
#ifdef HOLDALL_INSERT_TAIL
  p->next = NULL;
  if (ha->tail == NULL) {
    ha->head = p;
  } else {
    ha->tail->next = p;
  }
  ha->tail = p;
#else
  p->next = ha->head;
  ha->head = p;
#endif
  ha->count += 1;
  return 0;
}

size_t holdall_count(holdall *ha) {
  BUNCH_ENSURE(ha);                                                   /* <--- */
  return ha->count;
}

int holdall_apply(holdall *ha, int (*fun)(void *)) {
  BUNCH_ENSURE(ha);                                                   /* <--- */
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun(p->value);
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context(holdall *ha,
    void *context, void *(*fun1)(void *context, void *ptr),
    int (*fun2)(void *ptr, void *resultfun1)) {
  BUNCH_ENSURE(ha);                                                   /* <--- */
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(p->value, fun1(context, p->value));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context2(holdall *ha,
    void *context1, void *(*fun1)(void *context1, void *ptr),
    void *context2, int (*fun2)(void *context2, void *ptr, void *resultfun1)) {
  BUNCH_ENSURE(ha);                                                   /* <--- */
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(context2, p->value, fun1(context1, p->value));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

void holdall_dispose(holdall **haptr) {
  if (*haptr == NULL) {
    return;
  }
  BUNCH_ENSURE(*haptr);                                               /* <--- */
  choldall *p = (*haptr)->head;
  while (p != NULL) {
    choldall *t = p;
    p = p->next;
    free(t);
  }
  BUNCH_REMOVE(*haptr);                                               /* <--- */
  free(*haptr);
  *haptr = NULL;
}

//mem_swap : échange le contenu de deux zones mémoires de taille size pointées
//          par p1 et p2 qui ne se recouvrent pas
void mem_swap(choldall *p1, choldall *p2) {
  void *v = p1->value;
  p1->value = p2->value;
  p2->value = v;
}

//  partition_pivot : partitionne un tableau autour d'un pivot selon une
//    fonction de comparaison. Même sémantique des paramètres que pour la
//    fonction qsort déclarée dans l'en-tête <stdlib.h>. Le pivot est un
//    composant du tableau mais son choix n'est pas précisé dans cette
//    spécification.  Renvoie NULL si nmemb vaut 0, l'adresse finale du pivot
//    sinon.
size_t partition_pivot(holdall *base,
    int (*compar)(const void *, const void *)) {
  if (base->count == 0) {
    return 0;
  }
  choldall *p = base->head;
  size_t count = 1;
  choldall *q = base->head;
  choldall *k = base->head;
  for (size_t i = 1; i < base->count; i++) {
    k = k->next;
  }
  for (size_t j = 1; j <= base->count; j++) {
    if (compar(q->value, k->value) < 0) {
      mem_swap(p, q);
      p = p->next;
      count += 1;
    }
    q = q->next;
  }
  mem_swap(p, k);
  return count;
}

int holdall_sort(holdall *ha, int (*compar)(const void *, const void *)) {
  BUNCH_ENSURE(ha);
  if (ha->count == 0) {
    return -1;
  }
  choldall *h = ha->head;
  size_t compteur = ha->count;
  while (ha->count > 1) {
    size_t c = partition_pivot(ha, compar);
    if (c == 0) {
      return 0;
    }
    choldall *p = ha->head;
    for (size_t k = 1; k < c; k++) {
      p = p->next;
    }
    if (c - 1 < ha->count - c) {
      size_t cpt = ha->count;
      ha->count = c - 1;
      holdall_sort(ha, compar);
      ha->head = p->next;
      ha->count = cpt - c;
    } else {
      choldall *h = ha->head;
      ha->count = ha->count - c;
      ha->head = p->next;
      holdall_sort(ha, compar);
      ha->head = h;
      ha->count = c - 1;
    }
  }
  ha->head = h;
  ha->count = compteur;
  return 0;
}
