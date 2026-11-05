#ifndef DESCOMPACTAR_H
#define DESCOMPACTAR_H

#include "dicionario.h"
#include "huffman.h"
#include <stdio.h>


int descompactarArquivo(const char *arquivoEntrada, const char *arquivoSaida);
NoHuffman *lerArvore(FILE *entrada, int quantidadeTokens);
long obterTamanhoArquivo(const char *caminho);

#endif