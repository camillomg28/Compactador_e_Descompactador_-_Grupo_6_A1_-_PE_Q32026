#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "dicionario.h"



typedef struct NoHuffman {
	
	unsigned long frequencia;
	int indiceDicionario;
	struct NoHuffman *esquerda;
	struct NoHuffman *direita;
} NoHuffman;


NoHuffman *construirArvore(Dicionario *d);
NoHuffman **criarFolhas(Dicionario *d);
NoHuffman *combinarNos(NoHuffman *esquerda, NoHuffman *direita);
int encontrarMenorNo(NoHuffman **nos, int quantidade);
NoHuffman *criarNo(unsigned long frequencia, int indiceDicionario);
void imprimirArvore(NoHuffman *no);
void gerarCodigos(NoHuffman *no, char codigo[], int profundidade, Dicionario *d);
void liberarArvore(NoHuffman *no);


#endif