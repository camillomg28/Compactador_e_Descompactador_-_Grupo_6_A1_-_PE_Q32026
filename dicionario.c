#include "dicionario.h"
#include "hash.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//o arquivo dicionario.h e o hash.h importa todas as estruturas

void inicializarDicionario(Dicionario *d) {
    d->quantidade = 0; // o operador -> acessa os membros de uma estrutura, por exemplo d->quantidade acessa a variável quantidade da estrutura d
    d->capacidade = 10;

    d->palavras = malloc(d->capacidade * sizeof(Palavra)); //alocação dinâmica para o dicionário de palavras. Se não tiverem palavras, a capacidade vai a 0 e não gasta memória

    if (d->palavras == NULL) {
        d->capacidade = 0;
    }
}

int buscarPalavra(Dicionario *d, const char *texto) {
    int i;

    for (i = 0; i < d->quantidade; i++) {
        if (strcmp(d->palavras[i].texto, texto) == 0) { //strcmp é um comparador de strings, retorna 0 se forem iguais e outro numero se forem diferentes
            return i;
        }
    }

    return -1; //só retorna -1 (erro) quando não encontra palavras e faz o laço de repetição parar
}

int aumentarDicionario(Dicionario *d) {
    
    int novaCapacidade = d->capacidade * 2; //dobra a capacidade de alocação quando a quantidade de palavras não é suficiente
    Palavra *novo; //declara um ponteiro que é alocado na região de memória na região de memória que contém os elementos do tipo Palavra

    novo = realloc(d->palavras, novaCapacidade * sizeof(Palavra)); //realloc serve para aumentar ou diminuir o tamanho da alocação dinâmica sem perder os dados anteriores

    if (novo == NULL) { //se esse novo ponteiro for nulo, ou seja, não tiver mais local para realizar a alocação, faz a função parar e retornar 0
        return 0;
    }

    d->palavras = novo; // a variável da estrutura d chamada palavra recebe o valor novo
    d->capacidade = novaCapacidade;

    return 1;
}

int inserirPalavra(Dicionario *d, const char *texto, TabelaHash *h) { //recebe um ponteiro do tipo Dicionario *d
    int posicao = buscarHash(h, texto, d); 
    char *copia;

    if (posicao != -1) {
        d->palavras[posicao].frequencia++; //o operdador . acessa a variavel frequencia e adiciona +1, quando a posição existe (diferente de -1)
        return 1; //retorna 1 para a funcao parar
    }

    if (d->quantidade == d->capacidade) { //testa se toda a capacidade foi utilizada para poder aumentar o tamanho do dicionário
        if (!aumentarDicionario(d)) {
            return 0;
        }
    }

    copia = malloc(strlen(texto) + 1); //strlen(texto)+ 1 pois é necessário contar o \0 do final da string

    if (copia == NULL) { //testa se tem espaço na alocação dinâmica
        return 0;
    }

    strcpy(copia, texto); //cria uma cópia da palavra antes de buscar a próxima, serve para não perder o que cada byte vai significar na codificação

    posicao = d->quantidade;

    d->palavras[d->quantidade].texto = copia; //armazena essa cópia no dicionário 
    d->palavras[d->quantidade].frequencia = 1; //define a frequência como 1 para poder começar a contabilização
    d->palavras[d->quantidade].codigo = NULL;

    d->quantidade++; //informa que a quantidade de palavras no dicionário aumentou em 1

    if (!inserirHash(h, posicao, d)) {
    	free(d->palavras[posicao].texto);
	d->palavras[posicao].texto = NULL;
    	d->quantidade--;
    	return 0;
    }

    return 1;
}

int tamanhoCaractereUTF8(unsigned char primeiroByte){
	if((primeiroByte & 0x80) == 0) return 1;
	if((primeiroByte & 0xE0) == 0xC0) return 2;
	if((primeiroByte & 0xF0) == 0xE0) return 3;
	if((primeiroByte & 0xF8) == 0xF0) return 4;

	return 1;

}

int ehParteDaPalavra(unsigned char primeiroByte){
	if ((primeiroByte >= 'A' && primeiroByte <= 'Z')||(primeiroByte >= 'a' && primeiroByte <= 'z')||(primeiroByte>='0' && primeiroByte <= '9')) return 1;

	if(primeiroByte >= 0x80 && primeiroByte <= 0xF4) return 1;
	
	return 0;
	
}


int adicionarByte(char palavra[], int *tamanho, int capacidade, unsigned char byte) {
	if (*tamanho >= capacidade - 1) {
		return 0;
	}

	palavra[*tamanho] = byte;
	(*tamanho)++;
	palavra[*tamanho] = '\0';

	return 1;
}



int lerArquivo(const char *caminho, Dicionario *d, TabelaHash *h) {
    FILE *arquivo;
    int byte;
    int tamanho = 0;
    int capacidade = 100;
    int partePalavraAtual = -1;
    char *token = malloc(capacidade);

    if (token == NULL) return 0;

    arquivo = fopen(caminho, "rb");

    if (arquivo == NULL) {
        free(token);
        printf("Erro ao abrir o arquivo.\n");
        return 0;
    }

    token[0] = '\0';

    while ((byte = fgetc(arquivo)) != EOF) {
        unsigned char c = (unsigned char) byte;
        int partePalavra = ehParteDaPalavra(c);

        if (tamanho > 0 && partePalavra != partePalavraAtual) {
            if (!inserirPalavra(d, token, h)) {
                free(token);
                fclose(arquivo);
                return 0;
            }

            tamanho = 0;
            token[0] = '\0';
        }

        if (!adicionarByte(token, &tamanho, capacidade, c)) {
            free(token);
            fclose(arquivo);
            return 0;
        }

        partePalavraAtual = partePalavra;

        if (!partePalavra) {
            if (!inserirPalavra(d, token, h)) {
                free(token);
                fclose(arquivo);
                return 0;
            }

            tamanho = 0;
            token[0] = '\0';
            partePalavraAtual = -1;
        }
    }

    if (tamanho > 0) {
        if (!inserirPalavra(d, token, h)) {
            free(token);
            fclose(arquivo);
            return 0;
        }
    }

    free(token);
    fclose(arquivo);
    return 1;
}

void ordenarPorFrequencia(Dicionario *d) {
    int i, j;
    Palavra temporario;

    for (i = 0; i < d->quantidade - 1; i++) {
        for (j = 0; j < d->quantidade - 1 - i; j++) {

            if (d->palavras[j].frequencia <
                d->palavras[j + 1].frequencia) {

                temporario = d->palavras[j];
                d->palavras[j] = d->palavras[j + 1];
                d->palavras[j + 1] = temporario;
            }
        }
    }
}


