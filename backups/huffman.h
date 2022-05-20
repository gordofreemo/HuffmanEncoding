#ifndef HUFFMAN_H
#define HUFFMAN_H

/* Including stdio so we'll know about FILE type */
#include <stdio.h>

/**************************************************************/
/* Huffman encode a file.                                     */
/*     Also writes freq/code table to standard output         */
/* in -- File to encode.                                      */
/*       May be binary, so don't assume printable characters. */
/* out -- File where encoded data will be written.            */
/**************************************************************/
void encodeFile(FILE* in, FILE* out);

/***************************************************/
/* Decode a Huffman encoded file.                  */
/* in -- File to decode.                           */
/* out -- File where decoded data will be written. */
/***************************************************/
void decodeFile(FILE* in, FILE* out);

/* Represents both an element of the Huffman Tree and the Priority Queue */
struct SymbolNode
{
  unsigned long freq;
  unsigned char symbol; 
  unsigned int length;
  unsigned char code[256];
  struct SymbolNode *next; 
  struct SymbolNode *left;
  struct SymbolNode *right;
};

/* 
 * Given an array of frequencies for symbols
 * with the value at the ith indexing representing 
 * the frequency of the character w/ ASCII value i, 
 * generates the set of codes for them. 
 
 * unsigned long* freq - array of character frequencies
 * struct SymbolNode** root - Pointer to root of tree, used to clean up
 * memory in encode/decode file and useful for other operations.
 
 * return struct SymbolNode** - an array of generated symbol 
 * nodes, holding their respective codes. Index i of the array
 * represents the symbol with ascii code i
*/
struct SymbolNode** generateCodes(unsigned long* freq, struct SymbolNode** root);

/* 
 * Given the root of a huffman tree, frees all of the nodes for the given tree
 
 * struct SymbolNode* root - root of the huffman tree
*/
void freeTree(struct SymbolNode* root);

/*
 * Inserts into the tree a new symbol node. The new symbol node 
 * needs to have the correct code in order to be inserted. Creates new 
 * nodes along the way as needed. Call first with a depth of '0'
 
 * struct SymbolNode* root - root of huffman tree
 * struct SymbolNode* newNode - node to insert
 * int depth - pass in 0, but is used to keep place in code for the 
 * recursive calls. 
 
 * return struct SymbolNode* - used to update links in tree when making new nodes
*/
struct SymbolNode* insertTree(struct SymbolNode* root, struct SymbolNode* newNode, int depth);

struct SymbolNode* makeSymbol(unsigned long freq, unsigned char symbol); 

void printTree(struct SymbolNode* head, int level);

int isLeaf(struct SymbolNode* node);
#endif
