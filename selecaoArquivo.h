#ifndef SELECAOARQUIVO_H
#define SELECAOARQUIVO_H

#define TAMANHO_CAMINHO 260

int selecionarArquivoAbrir(char caminho[TAMANHO_CAMINHO]);
int selecionarArquivoSalvar(char caminho[TAMANHO_CAMINHO], const char *extensao);


#endif