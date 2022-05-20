#include <stdio.h> 
#include <stdlib.h> 
#include "huffman.h"

#define ARG_ERR 1
#define IN_FILE_ERR 2
#define OUT_FILE_ERR 3

unsigned long *countSymbols(FILE* inFile, unsigned long *totalSymbols);
void writeHeader(FILE* out, struct SymbolNode **codes);
void writeCode(FILE* out, struct SymbolNode *symbol);
void writeSymbols(FILE* in, FILE* out, struct SymbolNode **codes, unsigned long totalSymbols);

int main(int argc, char *argv[])
{
  FILE* inFile; 
  FILE* outFile; 

  /* ensuring validity of command-line inputs */
  if(argc != 3)
  {
    fprintf(stderr, "Command Line Argument Mismatch!\n");
    return ARG_ERR; 
  }
  
  inFile = fopen(argv[1], "rb");
  outFile = fopen(argv[2], "wb"); 

  /* making sure files can be opened */
  if(inFile == NULL)
  {
    fprintf(stderr, "Error Opening Input File %s!\n", argv[1]); 
    return IN_FILE_ERR; 
  }
  else if(outFile == NULL)
  {
    fprintf(stderr, "Error Opening Output File %s!\n", argv[2]);
    return OUT_FILE_ERR;
  }

  encodeFile(inFile, outFile); 
  fclose(inFile);
  fclose(outFile);
  return 0;
}

/* 
 * Count the occurence of symbols in a given file.
 
 * FILE* inFile - file to read from 
 * unsigned long *totalSymbols - pointer to an unsigned long that wiill hold the 
 * total number of characters seen in the file

 * returns an array of unsigned long, where index i
 * represents the occurences of the character with 
 * ascii value i inside of the file.
*/
unsigned long *countSymbols(FILE* inFile, unsigned long *totalSymbols)
{
  int currChar; 
  unsigned long *symbolCount = (unsigned long*)malloc(sizeof(unsigned long) * 256);
  *totalSymbols = 0;

  while((currChar = getc(inFile)) != EOF)
  {
    symbolCount[(unsigned char)currChar]++; 
    *totalSymbols += 1;
  }
  
  return symbolCount; 
}

/*
 * Outputs the information about how many symbols, symbol codes, 
 * and how many codes into the header of our output binary file.
 
 * FILE* out - binary file to write to 
 * struct SymbolNode **codes - array of nodes containing length of codes, codes
 * and symbol frequencies, ith index is the node representing symbol w/ASCII 
 * value i.
*/
void writeHeader(FILE* out, struct SymbolNode **codes)
{
  unsigned long numChars = 0; 
  unsigned char numSymbols = 0;
  unsigned int i;
  
  /* counting symbols + chars */
  for(i = 0; i < 256; i++)
  {
    if(codes[i] != NULL)
    {
      numSymbols++; 
      numChars += codes[i]->freq;
    }
  }
  
  fputc(numSymbols, out); /* write number of symbols */
  
  /* write codes + symbols */
  for(i = 0; i < 256; i++)
  {
    if(codes[i] != NULL)
    {
      fputc(i, out); /* write symbol value */ 
      writeCode(out, codes[i]);
    }
  }
  fwrite(&numChars, sizeof(unsigned long), 1, out); 
}

/*
 * Writes the binary representation of the code for a given symbol
 * to the output file, formatted correctly.
 
 * FILE* out - file to write to 
 * struct SymbolNode *symbol - SymbolNode holding the code and the length 
 * of the code that we will write to the file.
*/
void writeCode(FILE* out, struct SymbolNode *symbol)
{
  unsigned int numBytes = symbol->length/8 + 1; /* how many bytes to write */
  int i, j; /* loop variables */
  
  /* If symbol->length is already divisible by 8, the addition of 1 takes us over */
  if(symbol->length % 8 == 0) numBytes--;
  fputc(symbol->length, out);

  /* for every byte we need to write, generate that byte and write it*/
  for(i = 0; i < numBytes; i++)
  {
    unsigned char currByte = 0;
    /* put the code into a single byte */
    for(j = 0; j < 7; j++)
    {
      currByte |= symbol->code[i*8+j];
      currByte <<= 1;
    } 
    currByte |= symbol->code[i*8+j];
    fputc(currByte, out);
  }
}

/*
 * Writes to the output file the encoded version of each symbol
 * from the input file.
 
 * FILE* in - input file 
 * FILE* out - output file
 * struct SymbolNode **codes - array of the symbol nodes storing the codes, 
 * with the ith index being the symbol with ASCII value 'i'
 * unsigned long totalSymbols - how many total symbols are in the input file
*/
void writeSymbols(FILE* in, FILE* out, struct SymbolNode **codes, unsigned long totalSymbols)
{
  unsigned int byteLength = 8; 
  unsigned int currCodeLength = 0;
  struct SymbolNode *currNode = NULL; /* current symbol we are encoding */
  unsigned char currByte = 0; /* current byte we are encoding */
  int flag_last_loop = 0; /* used to ensure we finish placing the byte*/

  /* keep reading symbols while there are symbols to read */
  while(1)
  {
    /* if we have fully filled a byte, write it start writing to next byte */
    if(byteLength == 0)
    {
      fputc(currByte, out);
      currByte = 0;
      byteLength = 8;
    }
    /* if we have fully encoded a symbol, get the next symbol */ 
    if(currCodeLength == 0)
    {
      unsigned char nextChar = fgetc(in);
      totalSymbols--;
      if(flag_last_loop) break; /* once finished w/ last byte, break*/
      if(totalSymbols == 0) flag_last_loop = 1; /* still need to finish currByte */
      currNode = codes[nextChar]; 
      currCodeLength = currNode->length; 
    }
    
    /* shift the bits over, place the next bit of the code into byte*/
    currByte <<= 1;
    currByte |= currNode->code[currNode->length-currCodeLength];
    currCodeLength--;
    byteLength--;
  }
  
  /* if there is still a byte to place, pad the zeroes and place it */
  if(byteLength != 0 && byteLength != 8) 
  {
    int i;
    for(i = 0; i < byteLength; i++) currByte <<= 1;
    fputc(currByte, out);
  }
}

void encodeFile(FILE* in, FILE* out) 
{
  fpos_t startFile; /* start of the input file */
  unsigned long *symbolCount; /* array of symbol frequencies */
  struct SymbolNode **codes; /* array of SymbolNodes representing symbol + code */
  struct SymbolNode *treeRoot; /* pointer to root of huffman tree */
  unsigned long totalSymbols; /* how many characters in file */
  int i, j; /* loop indices */
  
  fgetpos(in, &startFile); /* keep track of start of in file*/
  symbolCount = countSymbols(in, &totalSymbols); /* generate frequency count */
  codes = generateCodes(symbolCount, &treeRoot); /* make huffman tree + codes */
  writeHeader(out, codes); /* write header to output */
  fsetpos(in, &startFile);  /* go to start of in file*/
  writeSymbols(in, out, codes, totalSymbols); /* encode all symbols and write */
  
  /* printing out the information table */
  printf("Symbol  Freq    Code\n");
  for(i = 0; i < 256; i++)
  {
    if(codes[i] != NULL && codes[i]->length != 0)
    {
      if(i < 33 || i > 126) printf("=%-7d", i); 
      else printf("%c       ", i); 
      printf("%-8lu", codes[i]->freq);
      for(j = 0; j < codes[i]->length; j++)
      {
        printf("%d", codes[i]->code[j]);
      }
      printf("\n");
    }
  }
  printf("Total chars = %lu", totalSymbols); 

  freeTree(treeRoot);
  free(codes);
  free(symbolCount);
}
