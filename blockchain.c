#include "header/blockchain.h"
#include <math.h>

CellTree* create_note(Block *b) {
    CellTree *retCT = (CellTree*)malloc(sizeof(CellTree));
    if(retCT == NULL) {
        printf("[create_node] Erreur d'allocation mémoire");
        return NULL;
    }
    retCT->block = b;
    retCT->father = NULL;
    retCT->firstChild = NULL;
    retCT->nextBro = NULL;
    retCT->height = 0;
}

int update_height(CellTree *father, CellTree *child) {
    if(father == NULL || child == NULL) {
        return 0;
    }
    if(father->height < child->height+1) {
        father->height = child->height+1;
        return 1;
    }
    return 0;
}

void add_child(CellTree *father; CellTree* child) {
    if(father == NULL || child == NULL) { return; }
    CellTree* pointerFather = father;
    CellTree *pointerCT = child;:
    if(father->child == NULL) {
        father->child = child;
    } else {
        // On cherche le premier frère libre
        pointerCT = father->child;
        while(pointerCT != NULL) {
            pointerCT = pointerCT->nextBro;
        }
        pointerCT = child;
    }
    // On va ensuite mettre à jour récursivement tous les father
    while(pointerFather != NULL) {
        update_height(pointerFather,pointerCT);
        pointerCT = pointerFather;
        pointerFather = pointerFather->father;
    }
}

void print_tree(CellTree *racine) {
    // On suppose qu'on affiche la racine , ses frères puis sa descendance
    if(racine == NULL) {
        return;
    }

    // On affiche le noeud courant 
    printf("Height : %d, \t Hash : %s\n", racine->height,racine->block->hash);

    // On fait un appel récursif sur nextBro et firstChild
    print_tree(racine->firstChild);
    print_tree(racine->nextBro);
}

void delete_node(CellTree* node) {
    if(node == NULL) {
        return;
    }
    node->firstChild->father = NULL;
    node->father->fistChild = node->nextBro;
    delete_block(node->block);
    free(node);
}

void delete_tree(CellTree* racine) {
    if(racine == NULL) {
        return;
    }
    delete_tree(racine->nextBro);
    delete_tree(racine->firstChild);
    delete_node(racine);
}

CellTree* highest_child(CellTree* cell) {
    // On prend les deux premiers fils de cell
    CellTree* retCellTree = cell->firstChild;
    CellTree* compCellTree = cell->firstChild->nextBro;

    // On compare en gardant le fils avec la plus grande hauteur a chaque itération
    while(compCellTree != NULL) {
        if(compCellTree->height > retCellTree->height) {
            retCellTree = compCellTree;
        }
        compCellTree = compCellTree->nextBro;
    }
    return retCellTree;
}

CellTree *last_node(CellTree* tree) {
    CellTree *lastCellTree = highest_child(tree);
    while(lastCellTree->height != 0) {
        lastCellTree = highest_child(lastCellTree);
    }
    return lastCellTree;
}

void submit_vote(Protected *p) {
    if(p == NULL) {
        printf("[submit_vote] Erreur argument non initialisé\n");
        return;
    }
    FILE *pending_vote = fopen("Pending_votes.txt","a");
    char *str = protected_to_str(p);
    fprintf(pending_vote, "%s\n", str);
    free(str);
}

void create_block(CellTree* tree, Key* author, int d) {
    CellTree *feuille = last_node(tree);
    
}