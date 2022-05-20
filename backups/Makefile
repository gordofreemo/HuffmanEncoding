all: huffencode huffdecode

clean:
	-rm huffencode huffdecode

huffencode: huffman.h huffencode.c treeBuilder.c
	gcc -g -Wall -ansi -pedantic -o huffencode huffman.h huffencode.c treeBuilder.c

huffdecode: huffman.h huffdecode.c treeBuilder.c
	gcc -g -Wall -ansi -pedantic -o huffdecode huffman.h huffdecode.c treeBuilder.c


