#ifndef CELLKEY_H
#define CELLKEY_H
#include "keys.h"

typedef struct cellKey{
    Key* data;
    struct cellKey* next;
}CellKey;

CellKey* create_cell_key(Key* key);
CellKey* headInsertCellKey(CellKey* CK, Key* key);
CellKey* read_public_keys(char* f);
void print_list_keys(CellKey* LCK);
void delete_cell_key(CellKey* cell);
void delete_list_keys(CellKey* LCK);
#endif
