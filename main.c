#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "dicionario.h"
#include "hash.h"
#include "huffman.h"
#include "compactador.h"
#include "descompactar.h"
#include "selecaoArquivo.h"


int main(void) {
    Dicionario d;
    TabelaHash h;
    int i;
    int querVisualizarFrequencia;
    int oqUserQuer;
    char arquivoEntrada[TAMANHO_CAMINHO];
    char arquivoSaida[TAMANHO_CAMINHO];


    do{
        printf("\n----------COMPACTADOR----------\n");
        printf("Digite um numero e pressione enter para:\n");
        printf("1 - Compactar arquivo\n");
        printf("2 - Descompactar arquivo\n");
        printf("3 - Sair\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &oqUserQuer) != 1) {
            printf("Entrada invalida.\n");
            return 1;
        }
    

        switch (oqUserQuer){
            case 1: 
                printf("Compactacao selecionada\n");
                break;
            
            case 2:
                printf("Descompactacao selecionada.\n");
                break;
            case 3:
                printf("Saindo...\n");
                return 0;
            default:
                printf("Opcao invalida.\n\n");
                continue;
        }
        
        if (oqUserQuer==1){
            printf("Digite 1 se quiser visualizar a frequencia de cada palavra no texto.\nDigite 0 caso contrario. Pressione enter para confirmar a resposta.\n\nResposta: ");
            if (scanf("%d", &querVisualizarFrequencia)!=1 || (querVisualizarFrequencia!=0 && querVisualizarFrequencia!=1)) {
                printf("Digite apenas 0 ou 1.\n");
                continue;
            }
            
            if (!selecionarArquivoAbrir(arquivoEntrada)) {
                printf("Selecao de arquivo cancelada.\n");
                continue;
            }
            
            if (!selecionarArquivoSalvar(arquivoSaida, ".sccp")) {
                printf("Selecao do destino cancelada.\n");
                continue;
            }
            
            
            inicializarDicionario(&d); //chama a função inicializarDicionario, definindo a capacidade e a quantidade das palavras e da alocação
        
        
            if (d.palavras == NULL) { //se a alocação das palavras não tiver espaço, para a função
                return 1;
            }
        
        
            if (!inicializarHash(&h, 1000)) {
                printf("Erro ao inicializar a tabela hash.\n");
                return 1;
            }
        
        
        
            if (!lerArquivo(arquivoEntrada, &d, &h)) {
                printf("Falha na leitura.\n");
        
                for (i = 0; i < d.quantidade; i++) {
                    free(d.palavras[i].texto);
                }
        
                free(d.palavras);
                liberarHash(&h);
                return 1;
            }
            
            /*for (i = 0; i < d.quantidade; i++) {
                printf("%s: %lu\n", d.palavras[i].texto, d.palavras[i].frequencia); //informa quantas vezes cada palavra aparece, para todas as palavras do dicionario
            }*/
        
            NoHuffman *raiz = construirArvore(&d);
        
            if (raiz==NULL) printf("Erro ao construir arvore.\n");
        
            else{ 
        
        	//imprimirArvore(raiz);
        
        
                char codigo[1000];
        
                gerarCodigos(raiz, codigo, 0, &d);
        
        	    clock_t inicio = clock();

                int sucesso = compactarArquivo(arquivoEntrada, arquivoSaida, &d, &h, raiz);
                
                clock_t fim = clock();
                
                double tempo = (double)(fim-inicio) / CLOCKS_PER_SEC;
                
                if (sucesso) {
                    long tamanhoOriginal = obterTamanhoArquivo(arquivoEntrada);
                    long tamanhoCompactado = obterTamanhoArquivo(arquivoSaida);
                
                    printf("\nArquivo original: %s Tamanho: %ld Bytes\n", arquivoEntrada, tamanhoOriginal);
                
                    printf("Arquivo comprimido: %s Tamanho: %ld Bytes\n", arquivoSaida, tamanhoCompactado);
                
                    if (tamanhoOriginal>0 && tamanhoCompactado>=0) {
                        double razao = (double)tamanhoCompactado/tamanhoOriginal;
                        printf("Razao de compressao (R = Sc/So): %.4f\n", razao);
                    }
                
                    printf("Tempo de compressao: %.6f s\n", tempo);
                }
        
        
        	    if (querVisualizarFrequencia == 1){
                    for (int i = 0; i < d.quantidade; i++) {
                        if (d.palavras[i].codigo == NULL) {
                            printf("Token: [%s] | Frequencia: %lu | Codigo nao gerado\n", d.palavras[i].texto, d.palavras[i].frequencia);
                        }
        
        	            else {
                            printf("Token: [%s] | Frequencia: %lu | Codigo: %s\n", d.palavras[i].texto, d.palavras[i].frequencia, d.palavras[i].codigo);
                        }
                    }
                }
        
                liberarArvore(raiz);
            }
        
        /*liberar memoria*/
        
            for (int i = 0; i < d.quantidade; i++) {
                free(d.palavras[i].texto);
                free(d.palavras[i].codigo);
            }
        
            free(d.palavras); //libera o vetor de palavras
            liberarHash(&h);
            
        }
        
        else if (oqUserQuer==2){
            if (!selecionarArquivoAbrir(arquivoEntrada)){
                printf("Selecao do arquivo compactado cancelado.\n");
                continue;
            }
            
            if (!selecionarArquivoSalvar(arquivoSaida, ".txt")){
                printf("Selecao do destino cancelado.\n");
                continue;
            }
            
            
            clock_t inicio = clock();
            int sucesso = descompactarArquivo(arquivoEntrada, arquivoSaida);
            clock_t fim = clock();
            double tempo = (double)(fim-inicio)/CLOCKS_PER_SEC;
            
            if (sucesso) {
                printf("Arquivo descompactado com sucesso!\n");
                long tamanhoCompactado = obterTamanhoArquivo(arquivoEntrada);
                long tamanhoDescompactado = obterTamanhoArquivo(arquivoSaida);
                
                printf("\nArquivo comprimido: %s Tamanho: %ld Bytes\n", arquivoEntrada, tamanhoCompactado);
                
                printf("Arquivo descompactado: %s Tamanho: %ld Bytes\n", arquivoSaida, tamanhoDescompactado);
                
                printf("Tempo de descompactacao: %.6f s\n", tempo);
            }
            else printf("Erro ao descompactar arquivo. Verifique se as extensoes sao compativeis e tente novamente.\n");
            
        }
    } while (1);


    
    return 0;
}
