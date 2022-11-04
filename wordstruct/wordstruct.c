#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FILE_IN "x"
#define NOT_IN_FILE "-"

struct hold {
  char *s;
  char *file;
  int oldFileNumber;
  int occurence;
  int ninfile;
  int *maxw;
  int nmaxfile;
  int *occlast;
  int *niflast;
  int is_same_number;
};

typedef struct hold hold;

hold *hold_empty(char *s, int argc, int k, int *max_word, int *occlast,
    int *niflast, int is_same_number) {
  hold *h = malloc(sizeof *h);
  if (h == NULL) {
    return NULL;
  }
  h->s = s;
  h->file = malloc((size_t) argc + 1);
  if (h->file == NULL) {
    free(h->s);
    free(h);
    return NULL;
  }
  *(h->file) = '\0';
  for (int x = 1; x <= k - 1; x++) {
    strcat(h->file, NOT_IN_FILE);
  }
  strcat(h->file, FILE_IN);
  h->occurence = 1;
  h->oldFileNumber = k;
  h->ninfile = 1;
  h->maxw = max_word;
  h->occlast = occlast;
  h->niflast = niflast;
  h->is_same_number = is_same_number;
  h->nmaxfile = argc;
  return h;
}

void hold_maj(hold *cptr, int k) {
  cptr->occurence += 1;
  if (cptr->oldFileNumber != k) {
    for (int x = cptr->oldFileNumber; x < k - 1; x++) {
      strcat(cptr->file, NOT_IN_FILE);
    }
    strcat(cptr->file, FILE_IN);
    cptr->ninfile += 1;
    cptr->oldFileNumber = k;
  }
}

int freeVal(hold *valptr) {
  free(valptr->file);
  free(valptr->s);
  free(valptr);
  return 0;
}

int holdcmp(hold *p1, hold *p2) {
  if (p1->ninfile < p2->ninfile) {
    return 1;
  } else if (p1->ninfile == p2->ninfile) {
    if (p1->occurence < p2->occurence) {
      return 1;
    } else if (p1->occurence == p2->occurence) {
      return strcmp(p1->s, p2->s);
    }
  }
  return -1;
}

int scptr_display(hold *s) {
  if (s->ninfile == 1) {
    return 0;
  }
  if (*(s->maxw) == 0) {
    if (s->is_same_number == 0) {
      return 0;
    }
    if (*(s->occlast) == s->occurence && *(s->niflast) == s->ninfile) {
      for (int x = s->oldFileNumber; x < s->nmaxfile - 1; x++) {
        strcat(s->file, NOT_IN_FILE);
      }
      return printf("%s\t%ld\t%s\n", s->file,
          (long int) (s->occurence), s->s) < 0;
    } else {
      return 0;
    }
  } else {
    *(s->maxw) -= 1;
    for (int x = s->oldFileNumber; x < s->nmaxfile - 1; x++) {
      strcat(s->file, NOT_IN_FILE);
    }
    *(s->occlast) = s->occurence;
    *(s->niflast) = s->ninfile;
    return printf("%s\t%ld\t%s\n", s->file,
        (long int) (s->occurence), s->s) < 0;
  }
}
