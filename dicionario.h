#ifndef DICIONARIO_H
#define DICIONARIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char *texto;
    unsigned long frequencia;
    char *codigo;
} Palavra;

typedef struct { 
    Palavra *palavras;
    int quantidade;
    int capacidade;
} Dicionario;

typedef struct TabelaHash TabelaHash;


void inicializarDicionario(Dicionario *d);
int buscarPalavra(Dicionario *d, const char *texto);
int aumentarDicionario(Dicionario *d);
int inserirPalavra(Dicionario *d, const char *texto, TabelaHash *h);
int tamanhoCaractereUTF8(unsigned char primeiroByte);
int adicionarByte(char palavra[], int *tamanho, int capacidade, unsigned char byte);
int ehParteDaPalavra(unsigned char primeiroByte);
int lerArquivo(const char *caminho, Dicionario *d, TabelaHash *h);
void ordenarPorFrequencia(Dicionario *d);


#endif