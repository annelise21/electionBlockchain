#include "header/hash.h"
#include "time.h"

void generate_random_data(int nv, int nc){
    FILE *keys;  
    FILE *candidates;
    FILE *declarations;
    Key* pKey;
    Key* sKey;
    Key* tabcandidats[nc];
    char buffer[256];
    char spkey[256];
    char sskey[256];
    // Generation de la liste d'Electeurs
    keys=fopen("keys.txt", "w"); 
    for(int i=0; i<nv ;i++){
        pKey=(Key*)malloc(sizeof(Key));
        sKey=(Key*)malloc(sizeof(Key));
        init_pair_keys(pKey,sKey,3,7);
        char* fpKey=key_to_str(pKey);
        char* fsKey=key_to_str(sKey);
        fprintf(keys,"\t%s , %s\t\n",fpKey,fsKey);
        free(fsKey);
        free(fpKey);
        free(sKey);
        free(pKey);
    }
    fclose(keys);

    // Generation de la liste des candidats
    candidates=fopen("candidates.txt","w");
    int tab[nv];
    for (int i=0; i<nv;i++){
        tab[i]=0;
    }
    for(int j=0; j<nc; j++){
        int ligne =rand() % nc;
        while (tab[ligne]==1){
            ligne= rand()%nc;
        }
        tab[ligne]=1;//actualise le tableau des candidats deja selectionnés
        keys=fopen("keys.txt","r");
        // selection du candidat au hasard
        for(int i=0; i<=ligne; i++){
            fgets(buffer,256,keys);
        }
        sscanf(buffer, "%s , %s",spkey,sskey);
        fprintf(candidates, "\t%s\n",spkey);
        tabcandidats[j] = str_to_key(spkey);
         
        fclose(keys);     
    }

    fclose(candidates);

    // Generations des déclariations de vote signées

    keys=fopen("keys.txt","r");
    declarations=fopen("declarations.txt","w" );

    for (int i=0; i<nv; i++){
        fgets(buffer,256,keys);
        sscanf(buffer, "%s , %s",spkey,sskey);
        // printf("\nspkey=%s,sskey=%s",spkey,sskey);
        Key* pkey=str_to_key(spkey);
        Key* skey= str_to_key(sskey);
        int val = rand()%nc;
        // printf("val = %d\n",val);
        char* mess= key_to_str(tabcandidats[val]);
        // printf("mess=%s\n",mess );
        // printf("pKey= (%llx,%llx)\n",pkey->N,pkey->keyValue);
        Signature* signE = sign(mess,skey);
        Protected* pr = init_protected(pkey, mess, signE);
        verify(pr);
        char* cpr= protected_to_str(pr);
        // printf("pr= %s\n",cpr);
        fprintf(declarations, "%s\n",cpr);
        free(pkey);
        free(skey);
        free(mess);
        free(signE);
        free(pr);
        free(cpr);
        
    }

    // free de tabcandidats

    fclose(keys);
    fclose(declarations);
}

int main(int argc, char** argv){
    srand(time(NULL));
    generate_random_data(100,2);

    // tests exercice 5
    // CellKey* LKC =  read_public_keys("candidates.txt");
    // print_list_keys(LKC);
    // delete_list_keys(LKC);
    
    CellProtected* declarations= read_protectedCell();
    printf("\nliste declarations:\n");
    printCellProtected(declarations);
    Protected* pl= malloc(sizeof(Protected));
    pl->mess=declarations->data->mess;
    pl->pKey=declarations->data->pKey;
    pl->sign=declarations->next->data->sign;
    declarations=headInsertCellProtected(pl,declarations);
    printf("\napres ajout\n");
    printCellProtected(declarations);
    declarations=keepValidCellProtected(declarations);
    printf("\napres supression:\n");
    printCellProtected(declarations);
    CellKey* voters = read_public_keys("keys.txt");
    printf("\nvotant:\n");
    print_list_keys(voters);
   
    CellKey* candidates= read_public_keys("candidates.txt");
    printf("\ncandidats:\n");
    print_list_keys(candidates);
    Key* gagnant= compute_winner(declarations,candidates,voters, 20, 200);
    char* g=key_to_str(gagnant);

    printf("\ngagnant: %s\n",g);
    

    
    
    

    return 0;
}