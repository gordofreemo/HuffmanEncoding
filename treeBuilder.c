/*
 * Andrew Geyko
 * This file is responsible for creating the huffman tree
 * data structure and allowing certain manipulations/operation 
 * on it. This file is not exposed to the user, but both the 
 * huffencode and huffdecode files use it in order to construct the huffman
 * tree.
*/
#include <stdio.h> 
#include <stdlib.h> 
#include "huffman.h" 

/* Function Declarations */
struct SymbolNode* makeSymbol(unsigned long freq, unsigned char symbol);
struct SymbolNode* insertPriority(struct SymbolNode* head, struct SymbolNode* insertNode);
unsigned char smallestValue(struct SymbolNode* root);
struct SymbolNode* buildTree(struct SymbolNode* head);
struct SymbolNode* combineNodes(struct SymbolNode* left, struct SymbolNode* right);
struct SymbolNode* popPriority(struct SymbolNode** head);
void fillCodes(struct SymbolNode* root, int direction, int depth, unsigned char* prevCode);
void printPriority(struct SymbolNode* head);
void printTree(struct SymbolNode* root, int level);

/* 
 * Creates a new symbolNode with given data 
 
 * unsigned long freq - frequency of given symbol
 * unsigned char symbol - actual symbol being held 
 
 * return symbolNode* - newly allocated node
*/
struct SymbolNode* makeSymbol(unsigned long freq, unsigned char symbol)
{
  int i;
  struct SymbolNode* newNode = (struct SymbolNode*)malloc(sizeof(struct SymbolNode));
  newNode->freq = freq; 
  newNode->symbol = symbol;
  newNode->length = 0;
  newNode->next = NULL; 
  newNode->left = NULL; 
  newNode->right = NULL; 
  for(i = 0; i < 256; i++) newNode->code[i] = 0;
  
  return newNode;
}

/* 
 * Inserts a new node into the priority queue into its correct place.
 * Uses the same algorithm as implemented in list and trees project.

 * struct SymbolNode* head - head of the priority queue 
 * struct SymbolNode* insertNode - node to insert into queue 
 
 * return SymbolNode* - new head of priority queue
*/
struct SymbolNode* insertPriority(struct SymbolNode* head, struct SymbolNode* insertNode)
{
  /* If head is empty or insertNode is less than head, insert before*/
  if(head == NULL || insertNode->freq < head->freq)
  {
    insertNode->next = head;
    return insertNode; 
  }
  /* If insert is strictly greater than head, keep going */
  else if(insertNode->freq > head->freq)
  {
    head->next = insertPriority(head->next, insertNode); 
  }
  /* If head and insert are equal, do the tie breaker */
  else
  {
    unsigned char insertSmall = smallestValue(insertNode);
    unsigned char headSmall = smallestValue(head); 
    if(insertSmall < headSmall) 
    {
      insertNode->next = head; 
      return insertNode; 
    }
    else head->next = insertPriority(head->next, insertNode);
  }

  return head; 
}

/*
 * Given a root of a tree, returns the smallest value 
 * in that tree. Useful for the tiebreaker in the priority queue.
 
 * struct SymbolNode* root - root of partial (or full) Huffman Tree
 
 * return unsigned char - leftmost (smallest) symbol in tree
*/
unsigned char smallestValue(struct SymbolNode* root)
{
  if(root->left == NULL) return root->symbol;
  else return smallestValue(root->left);
}

/*
 * Given a priority queue, reduces the priority queue into
 * a single node representing the Huffman Tree. 
 
 * struct SymbolNode* head - start of the priority queue 
 
 * returns SymbolNode* - root of the tree created
*/
struct SymbolNode* buildTree(struct SymbolNode* head)
{
  struct SymbolNode* leftChild; 
  struct SymbolNode* rightChild; 
  struct SymbolNode* combinedNode; 

  while(head->next != NULL)
  {
    leftChild = popPriority(&head); 
    rightChild = popPriority(&head); 
    combinedNode = combineNodes(leftChild, rightChild);
    head = insertPriority(head, combinedNode);
  }

  return head;
}

/*
 * Given two SymbolNodes (which are also trees), combines them together into 
 * a single tree, returning the new root containing the summed frequencies. 
 
 * struct SymbolNode* left - the left child of new tree
 * struct SymbolNode* right - the right child of new tree 
 
 * returns SymbolNode* - root of the new tree
*/
struct SymbolNode* combineNodes(struct SymbolNode* left, struct SymbolNode* right)
{
  unsigned long newFreq = left->freq + right->freq; 
  struct SymbolNode* newRoot = makeSymbol(newFreq, 'r');
  
  left->next = NULL;
  right->next = NULL; 
  
  newRoot->left = left; 
  newRoot->right = right;

  return newRoot;
}

/*
 * Pops the head off the priority queue and changes the head pointer 
 
 * struct SymbolNode** head - reference to the head of the queue 
 
 * returns SymbolNode* - node that got popped off
*/
struct SymbolNode* popPriority(struct SymbolNode** head)
{
  struct SymbolNode* poppedNode = *head; 
  *head = (*head)->next; 
  poppedNode->next = NULL; 
  return poppedNode; 
}

/*
 * Given a root of a huffman tree, generates the codes for them and fills 
 * the field in the structure holding the code. There is an array for easy
 * access of each of the symbol nodes so that we can retrieve them later 
 * without needing to handle the tree. Call this function with a depth of "-1"
 * to start.
 
 * struct SymbolNode* root - root of the huffman tree
 * int direction - 1 if we are going to the right
 *                 0 if we are going to the left
 * int depth - current depth of tree, useful for getting array index for code
 * unsigned char* prevCode - Code of the parent, copied
 * over into the child with an added value to represent the new direction 
*/
void fillCodes(struct SymbolNode* root, int direction, int depth, unsigned char* prevCode)
{
  if(root == NULL) return;
  else if(depth == -1)
  {
    fillCodes(root->left, 0, depth+1, root->code); 
    fillCodes(root->right, 1, depth+1, root->code);
  }
  else 
  {
    int i; 
    /* copying symbol code */
    for(i = 0; i < depth; i++)
    {
      root->code[i] = prevCode[i];
    }
    /* Updating code info for node and recursive call */
    root->length = depth+1; 
    root->code[depth] = direction;
    fillCodes(root->left, 0, depth+1, root->code); 
    fillCodes(root->right, 1, depth+1, root->code);
  }
}

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
struct SymbolNode** generateCodes(unsigned long* freq, struct SymbolNode** head)
{
  struct SymbolNode** symbols = (struct SymbolNode**)calloc(256, sizeof(struct SymbolNode*));
  struct SymbolNode* locHead = NULL;
  int i;

  for(i = 0; i < 256; i++)
  {
    if(freq[i] != 0)
    {
      struct SymbolNode* new = makeSymbol(freq[i], (unsigned char)i);
      locHead = insertPriority(locHead, new);
      symbols[i] = new;
    }
  }

  locHead = buildTree(locHead);
  fillCodes(locHead, 0, -1, locHead->code);
  
  *head = locHead;
  return symbols; 
}

/* 
 * Given a head to a priority queue, prints out 
 * information about all the nodes in the queue.
 
 * SymbolNode* head - head of priority queue
*/
void printPriority(struct SymbolNode* head)
{
  unsigned long freq;
  unsigned char symbol;

  if(head == NULL)
  {
  printf("\n");
  return; 
  }

  freq = head->freq; 
  symbol = head->symbol;
  
  if(symbol < 33 || symbol > 126) printf("=%d  %lu\n", symbol, freq);
  else printf("%c  %lu\n", symbol, freq); 
  printPriority(head->next); 
}

/*
 * Given a root of the huffman tree, prints
 * about some information about it.
 
 * struct SymbolNode* root - root of the tree
 * int level - used in recursive calls to print 
 * depth of node, pass in 0 or 1 in first call
*/
void printTree(struct SymbolNode* root, int level)
{
  int i;
  if(root->left == NULL && root->right == NULL)
  {
    printf("Leaf %c at Depth %d Code ", root->symbol, level);
    for(i = 0; i < root->length; i++) printf("%d", root->code[i]);
    printf("\n");
    return; 
  }
  printTree(root->left, level+1);
  printTree(root->right, level+1);
}

/* 
 * Given the root of a huffman tree, frees all of the nodes for the given tree
 
 * struct SymbolNode* root - root of the huffman tree
*/
void freeTree(struct SymbolNode* root)
{
  if(root == NULL) return; 
  freeTree(root->left);
  freeTree(root->right); 
  free(root);
}

/*
 * Inserts into the tree a new symbol node. The new symbol node needs to have
 * the correct code in order to be inserted. Creates new nodes along the way 
 * as needed. Call function with a depth of '0'.
 
 * struct SymbolNode* root - root of huffman tree
 * struct SymbolNode* newNode - node to insert 
 * int depth - pass in 0, but is used to keep place in code for the 
 * recursive calls.
 
 * return struct SymbolNode* - used to update links in tree when making new nodes 
*/
struct SymbolNode* insertTree(struct SymbolNode* root, struct SymbolNode* newNode, int depth)
{
  /* If we arrive at where the node should be placed, place it.
   * Create a new node if need be. */
  if(depth == newNode->length) return newNode;
  else if(root == NULL) root = makeSymbol(0, 'r');
  
  /* Go left or right depending on code, update root links */
  if(newNode->code[depth] == 0) root->left = insertTree(root->left, newNode, depth+1);
  else root->right = insertTree(root->right, newNode, depth+1); 

  return root; 
}

/*
 * Tests whether or not a given SymbolNode is a leaf on the huffman tree
 
 * struct SymbolNode* node - node to check
 
 * returns int - 1 if leaf
 *               0 if not leaf
*/
int isLeaf(struct SymbolNode* node)
{
  if(node->left == NULL && node->right == NULL) return 1; 
  else return 0;
}
