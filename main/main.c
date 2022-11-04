#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include "hashtable.h"
#include "holdall.h"
#include "wordstruct.h"

#define STR(s)  #s
#define XSTR(s) STR(s)

#define WORD_LENGTH_MAX 63

#define STR_LENGTH_MAX 256
#define MAX_WORD 10

#define OPTION_HELP 1
#define OPTION_PUNCTUATION 2
#define OPTION_UPPERCASE 3
#define OPTION_SAME_NUMBERS 4
#define OPTION_INITIAL 5
#define OPTION_TOP 6

#define VALUE_DEFAULT 0

#define CHAINE_STDIN "-"

typedef struct option option;

//Convertit la chaine associée à s en chaine entierement en majuscule.
void conv_maj(char *s);

//  str_hashfun : l'une des fonctions de pré-hachage conseillées par Kernighan
//    et Pike pour les chaines de caractères.
static size_t str_hashfun(const void *s);

//read_word: Lit un mot sur l'entrée standard caractere par caractere, le coupe
// si is_word_cut vaut 1, coupe si
// un caractère de ponctuation est croisée si is_punct_cut vaut 1, et
// mets la chaine en majuscule si jamais is_uppercase vaut 1. word_length_max
// sert à définir la taille actuelle de w, afin de savoir quand couper. Le mot
// est lu sur l'entrée stream.
int read_word(char **w, int is_word_cut, FILE *stream, size_t *word_length_max,
    int is_punct_cut, int is_uppercase);

int main(int argc, char *argv[]) {
  int r = EXIT_SUCCESS;
  hashtable *ht = hashtable_empty((int (*)(const void *, const void *))strcmp,
      (size_t (*)(const void *))str_hashfun);
  holdall *has = holdall_empty();
  if (ht == NULL
      || has == NULL) {
    return EXIT_FAILURE;
  }
  int *max_word = malloc(sizeof *max_word);
  if (max_word == NULL) {
    hashtable_dispose(&ht);
    holdall_dispose(&has);
    return EXIT_FAILURE;
  }
  *max_word = MAX_WORD;
  int *occlast = malloc(sizeof *occlast);
  if (occlast == NULL) {
    hashtable_dispose(&ht);
    holdall_dispose(&has);
    free(max_word);
    return EXIT_FAILURE;
  }
  *occlast = VALUE_DEFAULT;
  int *niflast = malloc(sizeof *niflast);
  if (niflast == NULL) {
    hashtable_dispose(&ht);
    holdall_dispose(&has);
    free(max_word);
    free(occlast);
    return EXIT_FAILURE;
  }
  *niflast = VALUE_DEFAULT;
  char opt;
  size_t word_length_max = WORD_LENGTH_MAX;
  int is_word_cut = true;
  int is_uppercase = false;
  int is_punct_cut = false;
  int is_same_number = false;
  static option longopt[] = {
    {"help", 0, NULL, OPTION_HELP},
    {"punctuation-like-space", 0, NULL, OPTION_PUNCTUATION},
    {"uppercasing", 0, NULL, OPTION_UPPERCASE},
    {"same-numbers", 0, NULL, OPTION_SAME_NUMBERS},
    {"initial", 1, NULL, OPTION_INITIAL},
    {"top", 1, NULL, OPTION_TOP},
    {0, 0, 0, 0}
  };
  while ((opt
        = (char) getopt_long(argc, argv, "i: p u t: s", longopt, NULL)) != -1) {
    if (opt == 'i' || opt == OPTION_INITIAL) {
      if (atoi(optarg) == 0) {
        is_word_cut = false;
      } else {
        word_length_max = (size_t) atoi(optarg);
      }
    }
    if (opt == 't' || opt == OPTION_TOP) {
      if (atoi(optarg) == 0) {
        *max_word = -1;
      } else {
        *max_word = atoi(optarg);
      }
    }
    if (opt == 'u' || opt == OPTION_UPPERCASE) {
      is_uppercase = true;
    }
    if (opt == 'p' || opt == OPTION_PUNCTUATION) {
      is_punct_cut = true;
    }
    if (opt == 's' || opt == OPTION_SAME_NUMBERS) {
      is_same_number = true;
    }
    if (opt == OPTION_HELP) {
      printf("Usage: ws [OPTION]... FILES: \n\n");
      printf("Print a list of words shared by text files.\n\n");
      printf("Program Information\n");
      printf("\t--help\tPrint this help message and exit.\n\n");
      printf("Input Control\n");
      printf(
          "  -i, --initial=VALUE\tSet the maximal number of significant initial letters\n");
      printf(
          "\t\tfor words to VALUE. 0 means without limitation. Default is 63.\n\n");
      printf(
          "  -p, --punctuation-like-space\tMake the punctuation characters play the same\n");
      printf("\t\trole as space characters in the meaning of words.\n\n");
      printf(
          "  -u, --uppercasing\tConvert each lowercase letter of words to the\n");
      printf("\t\tcorresponding uppercase letter.\n\n");
      printf("Output Control\n");
      printf(
          "  -s, --same-numbers\tPrint more words than the limit in case of same numbers.\n\n");
      printf(
          "  -t, --top=VALUE\tSet the maximal number of words to print to value.\n");
      printf("\t\t0 means all the words. Default is 10.\n\n");
      free(max_word);
      free(occlast);
      free(niflast);
      hashtable_dispose(&ht);
      holdall_dispose(&has);
      return EXIT_SUCCESS;
    }
  }
  char *w = malloc(word_length_max + 1);
  if (w == NULL) {
    goto error_capacity;
  }
  for (int k = optind; k < argc; ++k) {
    char *a = argv[k];
    FILE *fichier = NULL;
    if (strcmp(a, CHAINE_STDIN) == 0) {
      rewind(stdin);
      fichier = stdin;
    } else {
      fichier = fopen(a, "r");
      if (fichier == NULL) {
        goto error_file;
      }
    }
    int x = read_word(&w, is_word_cut, fichier, &word_length_max, is_punct_cut,
        is_uppercase);
    if (x == -1) {
      fclose(fichier);
      goto error_capacity;
    }
    while (x == 2) {
      x = read_word(&w, is_word_cut, fichier, &word_length_max, is_punct_cut,
          is_uppercase);
      if (x == -1) {
        fclose(fichier);
        goto error_capacity;
      }
    }
    while (x == 1) {
      void *cptr = (void *) hashtable_search(ht, w);
      if (cptr != NULL) {
        hold_maj(cptr, k - optind + 1);
      } else {
        char *s = malloc(strlen(w) + 1);
        if (s == NULL) {
          fclose(fichier);
          goto error_capacity;
        }
        strcpy(s, w);
        void *h = hold_empty(s, argc - optind + 1, k - optind + 1, max_word,
            occlast, niflast, is_same_number);
        if (h == NULL) {
          free(s);
          fclose(fichier);
          goto error_capacity;
        }
        if (holdall_put(has, h) != 0) {
          freeVal(h);
          fclose(fichier);
          goto error_capacity;
        }
        if (hashtable_add(ht, s, h) == NULL) {
          freeVal(h);
          fclose(fichier);
          goto error_capacity;
        }
      }
      x = read_word(&w, is_word_cut, fichier, &word_length_max, is_punct_cut,
          is_uppercase);
      if (x == -1) {
        fclose(fichier);
        goto error_capacity;
      }
      while (x == 2) {
        x = read_word(&w, is_word_cut, fichier, &word_length_max, is_punct_cut,
            is_uppercase);
        if (x == -1) {
          fclose(fichier);
          goto error_capacity;
        }
      }
    }
    if (fichier != stdin) {
      if (!feof(fichier)) {
        goto error_read;
      }
      fclose(fichier);
    }
  }
  holdall_sort(has, (int (*)(const void *, const void *))holdcmp);
  if (holdall_apply(has,
      (int (*)(void *))scptr_display) != 0) {
    goto error_write;
  }
#ifdef HASHTABLE_CHECKUP
  hashtable_display_checkup(ht, stderr);
#endif
  goto dispose;
error_file:
  fprintf(stderr, "*** Error: No file found.\n");
  goto error;
error_capacity:
  fprintf(stderr, "*** Error: Not enough memory.\n");
  goto error;
error_read:
  fprintf(stderr, "*** Error: A read error occurs.\n");
  goto error;
error_write:
  fprintf(stderr, "*** Error: A write error occurs.\n");
  goto error;
error:
  r = EXIT_FAILURE;
  goto dispose;
dispose:
  if (holdall_apply(has,
      (int (*)(void *))freeVal) != 0) {
    r = EXIT_FAILURE;
  }
  free(max_word);
  free(occlast);
  free(niflast);
  free(w);
  hashtable_dispose(&ht);
  holdall_dispose(&has);
  return r;
}

size_t str_hashfun(const void *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}

void conv_maj(char *s) {
  while (*s != '\0') {
    if (*s >= 'a' && *s <= 'z') {
      *s = (char) (*s - 32);
    }
    s++;
  }
}

int read_word(char **w, int is_word_cut, FILE *stream, size_t *word_length_max,
    int is_punct_cut, int is_uppercase) {
  int c = fgetc(stream);
  size_t k = 0;
  while (!isspace(c) && c != EOF) {
    if (is_punct_cut) {
      if (ispunct(c)) {
        break;
      }
    }
    *(*w + k) = (char) c;
    k += 1;
    c = fgetc(stream);
    if (k >= *word_length_max) {
      if (is_word_cut == false) {
        *w = realloc(*w, (*word_length_max) * 2);
        if (*w == NULL) {
          return -1;
        }
        *word_length_max = *word_length_max * 2;
      } else {
        break;
      }
    }
  }
  *(*w + k) = '\0';
  if (c == EOF) {
    return 0;
  }
  if (strcmp(*w, "") == 0) {
    return 2;
  }
  if (is_uppercase) {
    conv_maj(*w);
  }
  if (is_word_cut) {
    if (strlen(*w) == *word_length_max) {
      if (!isspace(c) && c != EOF) {
        if (is_punct_cut) {
          if (ispunct(c)) {
            return 1;
          }
        }
        fprintf(stderr, "*** Warning: Word '%s...' sliced.\n", *w);
        while (!isspace(c) && c != EOF) {
          if (is_punct_cut) {
            if (ispunct(c)) {
              break;
            }
          }
          c = fgetc(stream);
        }
      }
    }
  }
  return 1;
}
