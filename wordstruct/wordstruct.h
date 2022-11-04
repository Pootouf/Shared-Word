#ifndef WORDSTRUCT__H
#define WORDSTRUCT__H

//  struct hold, hold : structure permettant de gérer des mots, en leur
// associant la présence dans différents fichiers, leur nombre d'occurence, le
// dernier fichier dans lequel ce mot a été lu, le nombre de fichiers totaux,
// ainsi qu'un pointeur d'entier commun à tous les mots servant à l'affichage
// Ce n'est pas un module additionnel mais une extension du main, servant à la
// propreté du code
typedef struct hold hold;

//  hold_empty: crée un nouvel élément de la structure hold. Renvoie NULL en cas
// de dépassement de capacité, sinon renvoit un pointeur vers un controleur d'un
// élément de la structure. Les différents parametres correspondent aux
// différents champs de la structure. s correspond à la chaine s, argc au nombre
// de fichiers totaux (nmaxfile), k au fichier actuel lu (oldFileNumber),
// max_word au nombre maximal de mots pouvant etre affichés (maxw), occlast et
// niflast
// correspondent à des pointeurs sur les champs du dernier mot affiché
// occurrence et ninfile et enfin is_same_number sert à savoir si l'option -s
// est activé.
hold *hold_empty(char *s, int argc, int k, int *max_word, int *occlast,
    int *niflast, int is_same_number);

//  hold_maj: met à jour l'élement hold donné en paramètre. Pour cela, son champ
// occurence est mis à jour, son champ oldFileNumber également, et la chaine
// file aussi si nécessaire. Pour cela, k est utilisé afin de savoir si la mise
// à jour correspond à la lecture dans un nouveau fichier, k indiquant le numéro
// du fichier lu actuellement.
void hold_maj(hold *cptr, int k);

//  holdcmp: Compare différents éléments du type hold, d'abord sur leur présence
// dans différents fichiers, puis si égalité, sur leur nombre d'occurence, puis
// enfin, si égalité, sur un ordre lexicographique classique des chaines
// associés
// à la structure
int holdcmp(hold *p1, hold *p2);

// freeVal : Libere la structure associé à valptr
int freeVal(hold *valptr);

//  scptr_display : affiche sur la sortie standard la chaine associé à s servant
// à dire la présence ou non dans différents fichiers, le caractère
//tabulation, le nombre d'occurrence associé à s, le caractere tabulation,
// et enfin le mot associé à s.
//    Renvoie zéro en cas de succès, une valeur non nulle en cas d'échec.
int scptr_display(hold *s);

#endif
