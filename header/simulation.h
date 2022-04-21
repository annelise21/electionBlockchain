#ifndef SIMULATION_H
#define SIMULATION_H
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

void submit_vote(Protected* p);
void create_block(CellTree* tree, Key* author, int d);
void add_block(int d , char* name);
CellTree* read_tree();
Key* compute_winner_BT(CellTree* tree, CellKey* candidates, CellKey* voters, int sizeC, int sizeV);
#endif


