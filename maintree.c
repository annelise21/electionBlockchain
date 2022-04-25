#include "header/blockchain.h"

int main() {
    FILE *fb = fopen("temp.txt","r");
    Block *block = freadblock(fb);


    CellTree *tree = create_node(block);
    CellTree *child = create_node(block);

    //print_tree(tree);
    add_child(tree,child);
    //print_tree(tree);
    Key* k1= (Key*)malloc(sizeof(Key));
    Key* k2=(Key*)malloc(sizeof(Key));
    init_pair_keys(k1,k2,2,7);
    create_block(tree,k1,2);
    //add_block(2,"axelle");
    /*printf("highest child \n");
    print_tree(highest_child(tree));

    printf("last node \n");
    print_tree(last_node(tree));

    printCellProtected(tree->block->votes);
    delete_tree(tree,true);*/
}