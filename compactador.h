#ifndef COMPACTADOR_H
#define COMPACTADOR_H

#include "dicionario.h"
#include "huffman.h"
#include <stdio.h>

int gravarCodigo(FILE *saida, const char *codigo, unsigned char *byteAtual, int *bitsNoByte);
int finalizarGravacao(FILE *saida, unsigned char *byteAtual, int *bitsNoByte);
const char *buscarCodigo(Dicionario *d, TabelaHash *h, const char *token);
int compactarArquivo(const char *arquivoEntrada, const char *arquivoSaida, Dicionario *d, TabelaHash *h, NoHuffman *raiz);
int gravarArvore(FILE *saida, NoHuffman *raiz);
int gravarDicionario(FILE *saida, Dicionario *d);


#endif