//  Partie implantation du module __bunch__.

#include <stdlib.h>
#include <stdio.h>
#undef NDEBUG
#include "bunch.h"

typedef struct bunchitem bunchitem;

struct bunchitem {
  const char *file;
  int line;
  const char *func;
  const void *key;
  bunchitem *next;
};

static struct {
  int atexit;
  bunchitem *head;
  long long int nadd;
  long long int nremove;
  long long int nensure;
} __bunch__ = {
  .atexit = 0,
  .head = NULL,
  .nadd = 0,
  .nremove = 0,
  .nensure = 0,
};

static void bunch__atexit(void);
static _Noreturn void bunch__abort(const char *cause);

static _Noreturn void bunch__prierror(
  const char *file, int line, const char *func,
  const void *key);
static void bunch__prisection(const char *name);
static void bunch__priitem(
  const char *file, int line, const char *func,
  const void *key,
  const char *keymsg, const char *linemsg);

static bunchitem **bunch__search(const void *key);

void bunch__add(
    const char *file, int line, const char *func,
    const void *key) {
  __bunch__.nadd += 1;
  if (__bunch__.atexit == 0) {
    if (atexit(bunch__atexit) != 0) {
      bunch__abort("initialization");
    }
    __bunch__.atexit = 1;
  }
  if (*bunch__search(key) != NULL) {
    bunch__prierror(file, line, func, key);
  }
  bunchitem *p = malloc(sizeof *p);
  if (p == NULL) {
    bunch__abort("add");
  }
  *p = (bunchitem) {
    .file = file,
    .line = line,
    .func = func,
    .key = key,
    .next = __bunch__.head,
  };
  __bunch__.head = p;
}

void bunch__remove(
    const char *file, int line, const char *func,
    const void *key) {
  __bunch__.nremove += 1;
  bunchitem **pp = bunch__search(key);
  if (*pp == NULL || (*pp)->file != file) {
    bunch__prierror(file, line, func, key);
  }
  bunchitem *t = *pp;
  *pp = t->next;
  free(t);
}

void bunch__ensure(
    const char *file, int line, const char *func,
    const void *key) {
  __bunch__.nensure += 1;
  bunchitem **pp = bunch__search(key);
  if (*pp == NULL || (*pp)->file != file) {
    bunch__prierror(file, line, func, key);
  }
}

#define BUNCH__PRI(pre, format) pre "%16s" " " format "\n"

void bunch__atexit(void) {
  bunch__prisection("CALLS");
  fprintf(stderr,
      BUNCH__PRI("---", "%lld")
      BUNCH__PRI("---", "%lld")
      BUNCH__PRI("---", "%lld"),
      "add", __bunch__.nadd,
      "remove", __bunch__.nremove,
      "ensure", __bunch__.nensure);
  bunchitem *p = __bunch__.head;
  if (p != NULL) {
    bunch__prisection("WARNINGS");
    do {
      bunch__priitem(p->file, p->line, p->func, p->key,
          "key not removed", "added at line");
      bunchitem *t = p;
      p = p->next;
      free(t);
    } while (p != NULL);
  }
}

void bunch__abort(const char *cause) {
  fprintf(stderr, "Bunch %s operation failed: Not enough memory.\n", cause);
  abort();
}

void bunch__prierror(
    const char *file, int line, const char *func,
    const void *key) {
  bunch__prisection("FATAL ERROR");
  bunch__priitem(file, line, func, key,
      "illegal key", "at line");
  exit(EXIT_FAILURE);
}

void bunch__prisection(const char *name) {
  fprintf(stderr, "\nBUNCH %s\n", name);
}

void bunch__priitem(
    const char *file, int line, const char *func,
    const void *key,
    const char *keymsg, const char *linemsg) {
  fprintf(stderr,
      BUNCH__PRI("---", "%p")
      BUNCH__PRI("   ", "%d")
      BUNCH__PRI("   ", "%s")
      BUNCH__PRI("   ", "%s"),
      keymsg, key,
      linemsg, line,
      "in function", func,
      "from source", file);
}

bunchitem **bunch__search(const void *key) {
  bunchitem * const *pp = &__bunch__.head;
  while (*pp != NULL && (*pp)->key != key) {
    pp = &(*pp)->next;
  }
  return (bunchitem **) pp;
}
