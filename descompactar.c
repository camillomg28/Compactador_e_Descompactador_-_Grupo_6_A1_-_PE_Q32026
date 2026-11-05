#include "descompactar.h"
#include "dicionario.h"
#include "huffman.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

NoHuffman *lerArvore(FILE *entrada, int quantidadeTokens);



int descompactarArquivo(const char *arquivoEntrada, const char *arquivoSaida){
	
	unsigned long long totalBits;
	int quantidade;

	FILE *entrada = fopen(arquivoEntrada, "rb");

	if (entrada==NULL){
		printf("Erro ao abrir o arquivo compactado\n");
		return 0;
	}
	
	if (fread(&totalBits, sizeof(totalBits), 1, entrada)!=1){
		printf("Erro ao ler cabecalho de frequencias ou cabecalho incompativel\n");
		fclose(entrada);
		return 0;
	}
	
	if (fread(&quantidade, sizeof(int), 1, entrada)!=1 || quantidade<0){
	    printf("Erro ao ler arquivo na descompactacao.\n");
	    fclose(entrada);
	    
	    return 0;
	}

    Dicionario d;
    d.quantidade = quantidade;
    d.capacidade = quantidade;
    d.palavras = calloc(quantidade, sizeof(Palavra));
    
    if (quantidade>0 && d.palavras==NULL){
        printf("Erro ao alocar memoria para o dicionario.\n");
        fclose(entrada);
        return 0;
    }
    
    for (int i = 0; i < quantidade; i++) {
        unsigned int tamanho;
        if (fread(&tamanho, sizeof(tamanho), 1, entrada)!=1 || tamanho==0){
            printf("Erro ao ler o tamanho do token.\n");
            fclose(entrada);
            return 0;
        }
        
        d.palavras[i].texto = malloc((size_t)tamanho+1);
        if (d.palavras[i].texto==NULL) {
            printf("Erro ao alocar memoria para o token.\n");
            fclose(entrada);
            return 0;
        }
        if (fread(d.palavras[i].texto, 1, tamanho, entrada) != tamanho) {
            printf("Erro ao ler o token.\n");
            free(d.palavras[i].texto);
            fclose(entrada);
            return 0;
        }
    
    
    
        d.palavras[i].texto[tamanho] = '\0';
        d.palavras[i].frequencia = 0;
        d.palavras[i].codigo = NULL;
    }
    
    NoHuffman *raiz = lerArvore(entrada, quantidade);
    if (raiz==NULL){
        printf("Erro ao reconstruir a arvore de Huffman.\n");
        fclose(entrada);
        return 0;
    }
    
    FILE *saida = fopen(arquivoSaida, "wb");
    
    if (saida==NULL){
        printf("Erro ao criar o arquivo descompactado.\n");
        liberarArvore(raiz);
        fclose(entrada);
        return 0;
    }
    
    NoHuffman *atual = raiz;
    unsigned long long bitsLidos = 0;
    int byte;
    
    while (bitsLidos<totalBits){
        byte = fgetc(entrada);
        
        if (byte==EOF){
            printf("Erro. Fim inesperado do arquivo compactado.\n");
            fclose(saida);
            fclose(entrada);
            liberarArvore(raiz);
            return 0;
        }
        
        for (int i=7; i>=0 && bitsLidos<totalBits; i--){
            int bit = (byte>>i) & 1;
            
            if (raiz->esquerda==NULL && raiz->direita==NULL) atual = raiz;
            else {
                if (bit==0) atual = atual->esquerda;
                else atual = atual->direita;
            }
            
            if (atual==NULL){
                printf("Erro ao decodificar a arvore de Huffman na descompactacao.\n");
                fclose(entrada);
                fclose(saida);
                liberarArvore(raiz);
                return 0;
            }
            
            if (atual->esquerda==NULL && atual->direita==NULL){
                int indice = atual->indiceDicionario;
                const char *token = d.palavras[indice].texto;
                
                if(fwrite(token, 1, strlen(token), saida)!=strlen(token)){
                    
                    printf("Erro ao gravar o arquivo descompactado.\n");
                    fclose(saida);
                    fclose(entrada);
                    liberarArvore(raiz);
                    return 0;
                }
                
                atual = raiz;
            }
            
            bitsLidos++;
        }
        
        
    }
    
    
    
    fclose(saida);
	fclose(entrada);
	liberarArvore(raiz);
	
	for(int i=0; i<d.quantidade; i++){
	    free(d.palavras[i].texto);
	}
	
	free(d.palavras);
	
	return 1;


}

NoHuffman *lerArvore(FILE *entrada, int quantidadeTokens){
    int marcador = fgetc(entrada);
    
    if(marcador==EOF) return NULL;
    
    NoHuffman *no = malloc(sizeof(NoHuffman));
    
    if (no==NULL) return NULL;
    
    no->esquerda = NULL;
    no->direita = NULL;
    no->frequencia = 0;
    no->indiceDicionario = -1;
    
    if (marcador==1){
        int indice;
        
        
        if(fread(&indice, sizeof(int), 1, entrada)!=1 || indice<0 || indice>=quantidadeTokens){
            free(no);
            return NULL;
        }
        
        no->indiceDicionario = indice;
        return no;
        
        
    }
    
    if (marcador!=0){
        free(no);
        return NULL;
    }
    
    no->esquerda = lerArvore(entrada, quantidadeTokens);
    
    if (no->esquerda==NULL){
        free(no);
        return NULL;
    }
    
    no->direita = lerArvore(entrada, quantidadeTokens);
    
    if (no->direita==NULL){
        liberarArvore(no->esquerda);
        free(no);
        return NULL;
    }
    
    return no;
}


long obterTamanhoArquivo(const char *caminho) {
    FILE *arquivo = fopen(caminho, "rb");

    if (arquivo == NULL) {
        return -1;
    }

    if (fseek(arquivo, 0, SEEK_END) != 0) {
        fclose(arquivo);
        return -1;
    }

    long tamanho = ftell(arquivo);
    fclose(arquivo);

    return tamanho;
}







