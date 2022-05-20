/*
 * Andrew Geyko
 * This file is responsible for decoding a given 
 * file that was previously encoded by the huffencode program.
 * The program's command arguments are in the following format: 
 * ./huffdecode inputFile outputFile
 * Where inputFile is a file encoded by the huffman algorithm and 
 * outputFile is the file to write the decoded information to.
*/
#include <stdio.h>
#include "huffman.h"

struct SymbolNode* readHeader(FILE* in, int numSymbols, struct SymbolNode* root);
void decodeChars(FILE* in, FILE* out, int numChars, struct SymbolNode* root);

int main(int argc, char** argv)
{
  char* infile;
  char* outfile;
  FILE* in;
  FILE* out;

  if(argc != 3) 
  {
    printf("wrong number of args\n");
    return 1;
  }

  infile = argv[1];
  outfile = argv[2];

  in = fopen(infile, "rb");
  if(in == NULL)
  {
    printf("couldn't open %s for reading\n", infile);
    return 2;
  }

  out = fopen(outfile, "wb");
  if(out == NULL)
  {
    printf("couldn't open %s for writing\n", outfile);
    return 3;
  }

  decodeFile(in, out);

  fclose(in);
  fclose(out);

  return 0;
}

/***************************************************/
/* Decode a Huffman encoded file.                  */
/* in -- File to decode.                           */
/* out -- File where decoded data will be written. */
/***************************************************/
void decodeFile(FILE* in, FILE* out)
{
  struct SymbolNode* root;
  unsigned long numChars;
  int numSymbols = (unsigned int)fgetc(in); 
  if(numSymbols == 0) numSymbols = 256;

  root = readHeader(in, numSymbols, NULL); 

  fread(&numChars, sizeof(unsigned long), 1, in); 
  decodeChars(in, out, numChars, root);

  freeTree(root);
}

/*
  * Reads in the codes to the given symbols and generates a huffman
  * tree from them. Recursive method for fun.
  
  * FILE* in - file to read header from 
  * unsigned int numSymbols - how many symbols to read 
  * struct SymbolNode* root - pass in NULL to start, used in recursive calls
  
  * returns SymbolNode* - root of created huffman tree 
*/
struct SymbolNode* readHeader(FILE* in, int numSymbols, struct SymbolNode* root)
{
  unsigned char symbol, codeLength; 
  int i, j, numBytes;
  struct SymbolNode* newNode;
  if(numSymbols == 0) return root;
  
  /* Reading in information about next code */
  symbol = fgetc(in); 
  codeLength = fgetc(in);
  numBytes = (codeLength % 8) ? codeLength/8 + 1: codeLength/8;
  
  newNode = makeSymbol(0, symbol); 
  newNode->length = codeLength;
  
  /* Filling in Code*/
  for(i = 0; i < numBytes; i++)
  {
    unsigned char currByte = fgetc(in);
    for(j = 0; j < 8; j++)
    {
      int currBit = ((1 << (7-j)) & currByte) != 0;
      newNode->code[i*8+j] = currBit; 
    }
  }
  
  root = insertTree(root, newNode, 0);
  return readHeader(in, numSymbols-1, root); 
}

/* 
 * Decodes the input file and writes decoded characters to the output
 * file. Takes a completed huffman tree and how many chars to decode.
 
 * FILE* in - file to decode/read from
 * FILE* out - file to write decoded characters to 
 * int numChars- how many characters to decode
 * struct SymbolNode* root - root of huffman tree
*/
void decodeChars(FILE* in, FILE* out, int numChars, struct SymbolNode* root)
{
  unsigned char currByte;
  int currBit;
  struct SymbolNode* currNode = root;
  int byteLength = 0;

  while(numChars != 0)
  {
    if(byteLength == 0) 
    {
    currByte = fgetc(in);   
    byteLength = 8;
    }

    /* Getting current byte of code and moving in the tree */ 
    currBit = (currByte & (1 << (byteLength-1))) != 0;
    if(currBit == 1) currNode = currNode->right; 
    else currNode = currNode->left;

    if(isLeaf(currNode))
    {
      fputc(currNode->symbol, out); 
      currNode = root; 
      numChars--;
    }
    byteLength--;
  }
}
