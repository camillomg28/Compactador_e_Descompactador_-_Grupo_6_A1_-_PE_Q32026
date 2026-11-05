#include "compactador.h"
#include "dicionario.h"
#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int gravarCodigo(FILE *saida, const char *codigo, unsigned char *byteAtual, int *bitsNoByte){
	int i;
	
	for (i = 0; codigo[i] != '\0'; i++){
		*byteAtual = (*byteAtual << 1) | (codigo[i]-'0');
		(*bitsNoByte)++;
		if (*bitsNoByte==8){
			if (fputc(*byteAtual, saida)==EOF) return 0;
			*byteAtual = 0;
			*bitsNoByte = 0;
		}
	}
	
	return 1;
	
}






int finalizarGravacao(FILE *saida, unsigned char *byteAtual, int *bitsNoByte) {
	if (*bitsNoByte > 0) {
		*byteAtual <<= (8 - *bitsNoByte);

		if (fputc(*byteAtual, saida) == EOF) return 0;

		*byteAtual = 0;
		*bitsNoByte = 0;
		}

	return 1;
}







const char *buscarCodigo(Dicionario *d, TabelaHash *h, const char *token) {

    int indice = buscarHash(h, token, d);
    if (indice == -1) return NULL;
    
    return d->palavras[indice].codigo;
}






int compactarArquivo(const char *arquivoEntrada, const char *arquivoSaida, Dicionario *d, TabelaHash *h, NoHuffman *raiz){
    int byte;
    int tamanho = 0;
    int capacidade = 100;
    int partePalavraAtual = -1;
    unsigned char byteAtual = 0;
    int bitsNoByte = 0;
    
    FILE *entrada = fopen(arquivoEntrada, "rb");
    if (entrada==NULL) return 0;

    FILE *saida = fopen(arquivoSaida, "wb");
    if (saida==NULL){
	    fclose(entrada);
	    return 0;
    }

    char *token = malloc(capacidade);


    if (token==NULL){
	    fclose(entrada);
	    fclose(saida);
	    return 0;
    }

    token[0] = '\0';
    unsigned long long totalBits = 0;
    if (fwrite(&totalBits, sizeof(totalBits),1,saida)!=1 || !gravarDicionario(saida,d) || !gravarArvore(saida, raiz)){
	    free(token);
	    fclose(entrada);
	    fclose(saida);
	    return 0;
    }
    
    while ((byte = fgetc(entrada)) != EOF){
	    unsigned char c = (unsigned char) byte;
	    int partePalavra = ehParteDaPalavra(c);
	
	    if(tamanho>0 && partePalavra!=partePalavraAtual){
	    const char *codigo = buscarCodigo(d, h, token);
	        if (codigo == NULL) {
    		    printf("Token nao encontrado: [%s]\n", token);
                free(token);
    		    fclose(entrada);
    		    fclose(saida);
    		    return 0;
	        }

	        if (!gravarCodigo(saida, codigo, &byteAtual, &bitsNoByte)) {
    	        printf("Erro ao gravar codigo\n");
	            free(token);
    	        fclose(entrada);
    	        fclose(saida);
    	        return 0;
	        }
	    
	        totalBits += strlen(codigo);
	        tamanho = 0;
	        token [0] = '\0';

	    }
	
	    if (!adicionarByte(token, &tamanho, capacidade, c)){
    	    free(token);
    	    fclose(entrada);
    	    fclose(saida);
    	    return 0;
    	}
	
	    partePalavraAtual = partePalavra;

    	if (!partePalavra){
    
    	    const char *codigo = buscarCodigo(d, h, token);
    
    	    if (codigo==NULL){
        		free (token);
        		fclose(entrada);
    	        fclose(saida);
    	        return 0;
	        }


	    if (!gravarCodigo(saida, codigo, &byteAtual, &bitsNoByte)){
    		free (token);
    		fclose(entrada);
	        fclose(saida);
	        return 0;
	    }

	    totalBits += strlen(codigo);
	    tamanho = 0;
    	token[0] = '\0';
	    partePalavraAtual = -1;

	    }
	
    }

    if (tamanho>0){
	    const char *codigo = buscarCodigo(d,h,token);
    	if(codigo==NULL || !gravarCodigo(saida, codigo, &byteAtual, &bitsNoByte)){
	        free(token);
	        fclose(entrada);
	        fclose(saida);
	        return 0;
	    }
        totalBits += strlen(codigo);
    }

    


    if (!finalizarGravacao(saida, &byteAtual, &bitsNoByte)){
	    free(token);
	    fclose(entrada);
	    fclose(saida);
	    return 0;
    }


    if (fseek(saida, 0, SEEK_SET)!=0 || fwrite(&totalBits, sizeof(totalBits), 1, saida)!=1){
    	free(token);
    	fclose(entrada);
    	fclose(saida);
    	return 0;
    }

    fseek(saida, 0, SEEK_SET);
    fwrite(&totalBits, sizeof(totalBits), 1, saida);

    free(token);
    fclose(entrada);
    fflush(saida);
    fclose(saida);

    return 1;
}




int gravarArvore(FILE *saida, NoHuffman *raiz){
    if (raiz==NULL) return 0;
    if (raiz->esquerda==NULL && raiz->direita==NULL){
	    if (fputc(1,saida)==EOF) return 0;
	    if (fwrite(&raiz->indiceDicionario, sizeof(raiz->indiceDicionario), 1, saida)!=1) return 0;
	
	    return 1;
    }
    
    if (fputc(0, saida)==EOF) return 0;
    
    if (!gravarArvore(saida, raiz->esquerda)) return 0;
    if (!gravarArvore(saida, raiz->direita)) return 0;

    return 1;
}


int gravarDicionario(FILE *saida, Dicionario *d){

    int i;
    if (fwrite(&d->quantidade, sizeof(int), 1, saida)!=1) return 0;
    for (i=0; i<d->quantidade; i++){
	
	    unsigned int tamanho = (unsigned int)strlen(d->palavras[i].texto);
	    if (fwrite(&tamanho, sizeof(unsigned int), 1, saida)!=1) return 0;
	    if (fwrite(d->palavras[i].texto, 1, tamanho, saida)!=tamanho) return 0;
    }

    return 1;
}





