#ifdef _WIN32 //verifica qual o sistema operacional, se for windows executa o primeiro, caso contrário executa o outro.

#include <windows.h> //extensoes para abrir janelas no windows e selecionar os arquivos
#include <commdlg.h>
#include <string.h>

#include "selecaoArquivo.h"

int selecionarArquivoAbrir(char caminho[TAMANHO_CAMINHO]){

	OPENFILENAMEA ofn = {0};
	caminho[0] = '\0';

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = caminho;
	ofn.nMaxFile = TAMANHO_CAMINHO;
	ofn.lpstrFilter = "Todos os arquivos (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	
	return GetOpenFileNameA(&ofn) != 0;
}

int selecionarArquivoSalvar(char caminho[TAMANHO_CAMINHO], const char *extensao){
	
	OPENFILENAMEA ofn = {0};
	caminho[0] = '\0';

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = caminho;
	ofn.nMaxFile = TAMANHO_CAMINHO;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = extensao +1;
	return GetSaveFileNameA(&ofn) != 0;
	
	
		
}


#else

#include <stdio.h>
#include <string.h>
#include "selecaoArquivo.h"

int selecionarArquivoAbrir(char caminho[TAMANHO_CAMINHO]) {

    printf("Digite o caminho do arquivo que deseja abrir (maximo de 250 caracteres): ");

    return scanf(" %259[^\n]", caminho) == 1;
}

int selecionarArquivoSalvar(char caminho[TAMANHO_CAMINHO], const char *extensao) {

    printf("Digite o caminho para salvar o arquivo (maximo de 250 caracteres): ");

    if (scanf(" %259[^\n]", caminho) != 1)
        return 0;

    size_t tamanho = strlen(caminho);
    size_t tamanhoExtensao = strlen(extensao);

    if (tamanho < tamanhoExtensao ||
        strcmp(caminho + tamanho - tamanhoExtensao, extensao) != 0) {

        if (tamanho + tamanhoExtensao >= TAMANHO_CAMINHO)
            return 0;

        strcat(caminho, extensao);
    }

    return 1;
}

#endif