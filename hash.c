#include "header/hash.h"

CellProtected* sup_invalid_decla(CellProtected* LCP){
    // printf("je rentre\n");
    CellProtected* tmp;
    if(LCP== NULL){
        return NULL;
    }
    while(verify(LCP->data)==false){
        // printf("je rentre dans la boucle while\n");
        tmp=LCP;
        LCP=LCP->next;
        deleteCellProtected(tmp);
    }
    CellProtected* list = LCP;
    while( list->next!=NULL){
        if(verify(list->next->data)==false){
            // printf("je rentre dans la boucle while\n");
            tmp=list->next;
            list->next=list->next->next;
            deleteCellProtected(tmp);
        }
        list=list->next;
    }
    return LCP;
}
HashCell* create_hashcell(Key* key){
    HashCell* HC= (HashCell*)malloc(sizeof(HashCell));
    HC->key=key;
    HC->val= 0;
    return HC;
}
int hash_function(Key* key, int size){
    // printf("je rentre 5\n");
    long v1= key->keyValue;
    long v2= key->N;
    // printf("v1=%lx , v2=%lx, size= \n",v1,v2);
    int pos= (int)((v1+v2)+1)%size;
    // printf("je rentre 1\n");
    // printf("pos=%d",pos);
    return pos;

}

int find_position(HashTable* t , Key* key){
    // retourne -1 si la cle n'a pas ete trouvé et qu'il n'y a plus de places dans le tableau, la position de key si elle est presnte dans la table de hashage, la position ou devrait etre la cle sinon
    int pos= hash_function(key,t->size);
    // printf("je rentre 4\n");
    int i =0;
    while(i<t->size){
        // printf("je rentre 6\n");
        // printf("\n i=%d, size=%d\n, position:%d position sans transfert: %d\n",i,t->size, ((pos+i)%t->size), pos+i);
        
        if(t->tab[((pos+i)%t->size)]==NULL){
            // printf("la cle (%lx,%lx) n'est pas presente dans la table de hashage \n",key->keyValue,key->N);
            return ((pos+i)%t->size);
        }
        if(t->tab[((pos+i)%t->size)]->key->N== key->N && t->tab[((pos+i)%t->size)]->key->keyValue == key->keyValue){
            // printf("la cle (%lx,%lx) est presente dans la table de hashage\n", key->keyValue,key->N);
            return ((pos+i)%t->size);
        }
        i=i+1;
        // printf("i=%d\n",i);
    }
    // printf("je rentre 7\n");
    printf("la cle (%lx,%lx) n'est pas présente dans la table de hashage et il est impossible de l'inserer car le tableau est plein\n",key->N, key->keyValue);
    return -1;
}
HashTable* create_hashtable(CellKey* keys, int size){
    HashTable* t = (HashTable*)malloc(sizeof(HashTable));
    HashCell** tab = (HashCell**)malloc(sizeof(HashCell*)*size);
    // printf("je rentre 2 \n");
    CellKey* tmp= keys;
    int pos;
    t->size=size;
    t->tab=tab;
    // initialisation des elements du tableau à NULL
    for(int i=0;i<size;i++){
        t->tab[i]=NULL;
    }
    while(tmp!=NULL){
        // printf("je rentre 3 \n");
        pos=find_position(t,tmp->data);
        // printf("pos=%d",pos);
        if (pos!=-1){
            t->tab[pos]=create_hashcell(tmp->data);
        }
        tmp=tmp->next; 
    }
    return t;
}
void delete_hashtable(HashTable *t ){
    for( int i=0;i<t->size;i++){
        if(t->tab[i]!=NULL){
            free(t->tab[i]);
        }
    }
    free(t);
}
Key* compute_winner( CellProtected* decl, CellKey* candidates, CellKey* voters, int sizeC, int sizeV){
    //  printf("je rentre 1\n");
    HashTable* Hc = create_hashtable(candidates,sizeC);
    // printf("je rentre 1222\n");
    HashTable* Hv= create_hashtable(voters,sizeV);
    int posV;
    int posC;
    Key* pkeyc; 
    Key* pkeyv;
    int nbvotes=-1;
    Key* gagnant;
    //  printf("je rentre 10\n");
    while (decl !=NULL){
        // printf("je rentre 11\n");
         pkeyv= decl->data->pKey;
         posV= find_position(Hv,pkeyv);
        //  on verifie que le candidat n'a pas deja voté 
         if(Hv->tab[posV]->val==0){
            // printf("je rentre 12\n");
             pkeyc= str_to_key(decl->data->mess);
             posC= find_position(Hc,pkeyc);
            //  on verifie que le candidat pour lequel a voté le citioyen existe 
             if(Hc->tab[posC]!=NULL){
                 Hv->tab[posV]->val=1;
                 Hc->tab[posC]->val=1+Hc->tab[posC]->val;
                //  printf("je rentre 13\n");
             }
         }        
        decl= decl->next;
    }
    for(int i=0; i<sizeC; i++){

    
        if ( Hc->tab[i]!=NULL && Hc->tab[i]->val >= nbvotes){
            // printf("je rentre 22\n");
            nbvotes=Hc->tab[i]->val;
            gagnant=Hc->tab[i]->key;
        }
    }
    return gagnant;
}