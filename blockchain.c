#include "header/blockchain.h"
#include "header/block.h"
#include "header/cellkey.h"
#include "header/keys.h"
#include <math.h>
#include <dirent.h>
#include <stdio.h>

CellTree* create_node(Block *b) {
    CellTree *retCT = (CellTree*)malloc(sizeof(CellTree));
    if(retCT == NULL) {
        printf("[create_node] Erreur d'allocation mémoire");
        return NULL;
    }
    retCT->block = b;
    retCT->father = NULL;
    retCT->child = NULL;
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

void add_child(CellTree *father, CellTree* child) {
    if(father == NULL || child == NULL) { return; }
    CellTree* pointerFather = father;
    CellTree *pointerCT = child;
    if(father->child == NULL) {
        father->child = child;
    } else {
        // On cherche le premier frère libre
        pointerCT = (CellTree*)father->child;
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

    // On fait un appel récursif sur nextBro et child
    print_tree(racine->child);
    print_tree(racine->nextBro);
}

void delete_node(CellTree* node) {
    if(node == NULL) {
        return;
    }
    node->child->father = NULL;
    node->father->child = node->nextBro;
    delete_block(node->block);
    free(node);
}

void delete_tree(CellTree* racine,bool father) {
    if(racine == NULL) {
        return;
    }
    if(father) {
        delete_tree(racine->nextBro,false);
    }
    delete_tree(racine->child,false);
    delete_node(racine);
}

CellTree* highest_child(CellTree* cell) {
    // On prend les deux premiers fils de cell
    CellTree* retCellTree = cell->child;
    CellTree* compCellTree = cell->child->nextBro;

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
void create_block(CellTree* tree, Key* author, int d){
    Block* block=(Block*)malloc(sizeof(Block));
    CellProtected* cp= read_protectedCell("Pending_votes.txt");
    remove("Pending_votes.txt");
    block->votes= cp;
    Key* auteur= (Key*)malloc(sizeof(Key));
    init_key(auteur,author->keyValue,author->N);
    block->author=auteur;
    CellTree* t= last_node(tree);
    Block* lastBlock= t->block;
    block->previous_hash=lastBlock->hash;
    compute_proof_of_work(block,d);
    FILE* f= fopen("Pending_block.txt","w");
    fprintblock(f,block);  
}
void add_block(int d , char* name){
    FILE* f= fopen("Pending_block.txt","r");
    Block* block= freadblock(f);
    char nomfic[256];
    fclose(f);
    remove("Pending_block.txt");
    if (block!=NULL){
        if (verify_block(block,d)==true){
            strcpy(nomfic,"./Blockchain/");
            strcat(nomfic,name);
            // on suppose que le repertoire BlockChain existe deja
            FILE* name= fopen(nomfic,"w");
            if (name!=NULL){
                fprintblock(name,block);
                fclose(name);
            } 
        }
        delete_block(block);
    }

}
CellTree* read_tree(){
    // on suppose que la taille du tableau est superieur au nombre de fichier lu dans Blockchain
    CellTree** T = (CellTree**)calloc(256,sizeof(CellTree*));
    DIR *rep = opendir("./Blockchain/");
    char nomfic[256];
    FILE* f;
    int i =0;
    Block* b;
    if (rep !=NULL){
        struct dirent * dir;
        while (( dir == readdir(rep))) {
            if (strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..") != 0){
                printf("Chemin du fichier : ./Blockchain/%s \n", dir->d_name);
                strcpy(nomfic,"./Blockchain/");
                strcat(nomfic,dir->d_name);
                f= fopen(nomfic,"r");
                if (f!=NULL){
                    b= freadblock(f);
                    T[i]= create_node(b);
                    i++;
                }
            }
            closedir(rep);


        }
    }
    int taille= i+1;
    int j;
    for( i=0; i<taille; i++){
        for (j=0; i<taille; j++){
            if( i!=j && strcmp(T[j]->block->previous_hash,T[i]->block->hash) == 0 ){
                add_child(T[i],T[j]);
            }
            j++;
        }
        j=0;

    }
    for (int k=0; k<taille; k++){
        if (T[k]->father==NULL){
            return T[k];
        }
    }
    printf("il y a une erreur quelque part\n");
    return NULL;

}
Key* compute_winner_BT(CellTree* tree, CellKey* candidates, CellKey* voters, int sizeC, int sizeV){

    CellProtected* declarations= NULL;
    CellTree* T = highest_child(tree);
    while(T!=NULL){
        declarations=fusion(declarations,T->block->votes);
        T=T->child;
    }
    declarations=keepValidCellProtected(declarations);
    return compute_winner(declarations,candidates,voters,sizeC,sizeV);
}
      