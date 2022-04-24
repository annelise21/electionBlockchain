#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include "block.h"
#include "blockchain.h"
#include "cellkey.h"
#include "crabe.h"
#include "hash.h"
#include "keys.h"
#include "protected.h"
#include "protectedcell.h"
#include "rsa.h"
#include "sign.h"

typedef struct _block_tree_cell{
    Block *block;
    struct block_tree_cell *father;
    struct block_tree_cell *firstChild;
    struct block_tree_cell *nextBro;
    int height;
}CellTree;

CellTree* create_node(Block *b);
int update_height(CellTree *father, CellTree *child);
void add_child(CellTree *father; CellTree* child);
void print_tree(CellTree *racine);
void delete_node(CellTree* node);
void delete_tree(CellTree* racine, bool father);
CellTree* highest_child(CellTree* cell);
void submit_vote(Protected *p);
CellTree *last_node(CellTree* tree);
void create_block(CellTree* tree, Key* author, int d);
void add_block(int d , char* name);
Key* compute_winner_BT(CellTree* tree, CellKey* candidates, CellKey* voters, int sizeC, int sizeV);
#endif
