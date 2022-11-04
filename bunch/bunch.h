//  Partie interface du module bunch.

//  Le module bunch permet de tester toute adresse afin de s'assurer qu'elle
//    appartient à un ensemble d'adresses autorisées dit « trousseau de clés »
//    dans la suite.

//  #include "bunch.h"
//  void BUNCH_ADD(void *key);
//  void BUNCH_REMOVE(void *key);
//  void BUNCH_ENSURE(void *key);

//  L'en-tête "bunch.h" définit les trois macrofonctions BUNCH_ADD, BUNCH_REMOVE
//    et BUNCH_ENSURE dont l'unique paramètre est supposé être une adresse. Les
//    macrofonctions sont expansées en des expressions de type void.
//  Le trousseau est initialement vide. Les macrofonctions BUNCH_ADD,
//    BUNCH_REMOVE et BUNCH_ENSURE permettent respectivement d'ajouter une clé
//    au trousseau, de retirer une clé du trousseau et d'être assuré qu'une clé
//    appartient bien au trousseau.
//  Les cas d'erreur sont les suivants :
//  - les ressources ne sont pas suffisantes pour installer le trousseau ;
//  - la clé à lui ajouter lui appartient déjà ;
//  - les ressources ne sont pas suffisantes pour pouvoir lui ajouter une clé ;
//  - la clé à lui retirer ou à tester ne lui appartient pas ;
//  - la clé à lui retirer ou à tester lui appartient mais l'appel à
//    BUNCH_REMOVE ou à BUNCH_ENSURE ne figure pas dans le même fichier source
//    que l'appel à BUNCH_ADD qui a ajouté la clé.

//  Sauf en cas d'erreur due aux ressources, un message d'information puis,
//    éventuellement, un message d'avertissement sont envoyés sur la sortie
//    erreur lors d'une terminaison normale du programme. Le message
//    d'information fait état du nombre d'appels aux différentes macrofonctions.
//    Le message d'avertissement fait état des clés encore dans le trousseau
//  En cas d'erreur, un message d'erreur est envoyé sur la sortie. S'il s'agit
//    d'une erreur due aux ressources, la fin anormale du programme est
//    provoquée (appel à abort). Sinon, le message d'erreur est envoyé avant les
//    messages d'information et d'avertissement ; une terminaison normale est
//    ensuite provoquée avec transmission de la valeur EXIT_FAILURE à
//    l'environnement d'exécution (appel à exit) ; le message indique la clé
//    fautive puis, relativement à l'appel de la macrofonction, le nom du
//    fichier source dans lequel il figure, le numéro de la ligne à laquelle il
//    y figure et le nom de la fonction dans lequel il figure.

//  Le fonctionnement du module suit celui du module standard assert en étant
//    sensible à la macroconstante NDEBUG. Si NDEBUG est définie lors de
//    l'inclusion de l'en-tête "bunch.h", les trois macrofonctions BUNCH_ADD,
//    BUNCH_REMOVE et BUNCH_ENSURE sont inopérantes. Ces trois macrofonctions
//    sont définies ou redéfinies selon l'état courant de NDEBUG à chaque fois
//    que l'en-tête "bunch.h" est inclus.

#ifdef BUNCH__H

#undef BUNCH__H
#undef BUNCH__CALL

#undef BUNCH_ADD
#undef BUNCH_REMOVE
#undef BUNCH_ENSURE

#endif

#define BUNCH__H

#ifdef NDEBUG

#define BUNCH__CALL ((void) 0)

#define BUNCH_ADD(key)    BUNCH__CALL
#define BUNCH_REMOVE(key) BUNCH__CALL
#define BUNCH_ENSURE(key) BUNCH__CALL

#else

#define BUNCH__DECLARE(oper)                                                   \
  extern void bunch__ ## oper(                                                 \
    const char *file, int line, const char *func,                              \
    const void *key)

BUNCH__DECLARE(add);
BUNCH__DECLARE(remove);
BUNCH__DECLARE(ensure);

#define BUNCH__CALL(oper, key) \
  bunch__ ## oper(__FILE__, __LINE__, __func__, key)

#define BUNCH_ADD(key)    BUNCH__CALL(add, key)
#define BUNCH_REMOVE(key) BUNCH__CALL(remove, key)
#define BUNCH_ENSURE(key) BUNCH__CALL(ensure, key)

#endif
