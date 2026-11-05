
CC = gcc
CFLAGS = -Wall -Wextra -g

# Detecta o sistema operacional, se é Windows ou se é Linux
ifeq ($(OS),Windows_NT)
    LDLIBS = -lcomdlg32
    TARGET = compactador.exe
else
    LDLIBS =
    TARGET = compactador
endif

OBJECTS = main.o dicionario.o hash.o huffman.o compactador.o descompactar.o selecaoArquivo.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDLIBS)

main.o: main.c dicionario.h hash.h huffman.h compactador.h descompactar.h selecaoArquivo.h
	$(CC) $(CFLAGS) -c main.c -o main.o

dicionario.o: dicionario.c dicionario.h hash.h
	$(CC) $(CFLAGS) -c dicionario.c -o dicionario.o

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c -o hash.o

huffman.o: huffman.c huffman.h dicionario.h
	$(CC) $(CFLAGS) -c huffman.c -o huffman.o

compactador.o: compactador.c compactador.h dicionario.h hash.h huffman.h
	$(CC) $(CFLAGS) -c compactador.c -o compactador.o

descompactar.o: descompactar.c descompactar.h dicionario.h huffman.h
	$(CC) $(CFLAGS) -c descompactar.c -o descompactar.o

selecaoArquivo.o: selecaoArquivo.c selecaoArquivo.h
	$(CC) $(CFLAGS) -c selecaoArquivo.c -o selecaoArquivo.o

clean:
	rm -f $(OBJECTS) compactador compactador.exe

.PHONY: all clean
