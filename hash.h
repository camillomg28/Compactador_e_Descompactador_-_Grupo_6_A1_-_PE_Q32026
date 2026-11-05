#ifndef HASH_H
#define HASH_H

#include "dicionario.h"

typedef struct NoHash {
    int indiceDicionario;
    struct NoHash *prox;
} NoHash;

typedef struct TabelaHash TabelaHash;

struct TabelaHash {
    NoHash **buckets;
    int capacidade;
};

unsigned int funcaoHash(const char *texto, int capacidade);
int inicializarHash(TabelaHash *h, int capacidade);
int buscarHash(TabelaHash *h, const char *texto, Dicionario *d);
int inserirHash(TabelaHash *h, int indiceDicionario, Dicionario *d);

void liberarHash(TabelaHash *h);

#endif