#include <time.h>
#include "header/rsa.h"
#include "header/keys.h"
#include "header/protected.h"
#include "header/sign.h"
#include "header/cellkey.h"
#include "header/protectedcell.h"
#include <stdbool.h>

void print_long_vector(long *result, int size) {
    printf("Vector; [");
    for(int i=0; i < size; i++) {
        printf("%lx \t", result[i]);
    }
    printf("]\n");
}

void generate_random_data(int nv, int nc) {
    if(nv == 0 || nc == 0) {
        printf("Args Error\n");
        return;
    }
    FILE *fv = fopen("keys.txt","w");
    FILE *fc = fopen("candidates.txt","w");
    FILE *fd = fopen("declarations.txt","w");
    int* tabIndice = (int*)calloc(nv,sizeof(int));
    if(tabIndice == NULL) {
        printf("Erreur dans l'allocation d'un tableau d'indice, impossible de générer des candidats. \n");
        return;
    }
    Key* tab_public_keys[nv];
    Key* tab_secret_keys[nv];
    Key* tab_candidate_keys[nc];

    // (public,secret) keys generation
    for(int i = 0; i < nv; i++) {
        Key* pKey = (Key*)malloc(sizeof(Key));
        Key* sKey = (Key*)malloc(sizeof(Key));
        init_pair_keys(pKey,sKey,2,7);
        fprintf(fv,"(%lx,%lx) (%lx,%lx)\n", pKey->keyValue, pKey->N, sKey->keyValue,sKey->N);
        tab_public_keys[i] = pKey;
        tab_secret_keys[i] = sKey;
    }

    // Candidates selection
    for(int i = 0; i < nc; i++) {
        int pos = rand()%nc;
        while(tabIndice[pos] == 1) {
            pos = rand()%nc;
        }
        tab_candidate_keys[i] = tab_public_keys[pos];
        tabIndice[pos] = 1;
        fprintf(fc,"(%lx,%lx)\n", tab_candidate_keys[i]->keyValue, tab_candidate_keys[i]->N);
    }

    // Declarations
    for(int i = 0; i < nv; i++) {
        int idCandidate = rand()%nc;
        char *vote = key_to_str(tab_candidate_keys[idCandidate]);
        Signature *voteSign = sign(vote, tab_secret_keys[i]);
        Protected *voteProtected = init_protected(tab_public_keys[i],vote,voteSign);
        char *declaration = protected_to_str(voteProtected);
        fprintf(fd,"%s\n", declaration);
        freeSignature(voteSign);
        free(voteProtected->mess);
        free(voteProtected);
        free(declaration);
    }
    for(int i = 0; i < nv; i++) {
        free(tab_public_keys[i]);
    }

    for(int i = 0; i < nv; i++) {
        free(tab_secret_keys[i]);
    }
    free(tabIndice);
    fclose(fv);
    fclose(fc);
    fclose(fd);
}

int main() {
    srand(time(NULL));

    // Key generation
    long p = random_prime_number(3,7,5000);
    long q = random_prime_number(3,7,5000);
    while(p==q) {
        q = random_prime_number(3,7,5000);
    }
    long n, s, u;
    generate_key_values(p,q,&n,&s,&u);
    // To get positive keys :
    if(u < 0) {
        long t = (p-1)*(q-1);
        u = u+t; // On aura toujours s*u mod t = 1
    }

    // printing keys in hexadecimal format
    printf("Clé publique = (%lx,%lx) \n",s,n);
    printf("Clé privée = (%lx,%lx) \n", u,n);

    // Encryption:
    char mess[210] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam ex odio, finibus at dui eu, dignissim dictum neque. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos fusce.";
    int len = strlen(mess);
    long *crypted = encrypt(mess,s,n);

    printf("Initial message: %s \n", mess);
    printf("Encoded representation : \n");
    print_long_vector(crypted,len);

    // Decryption
    char *decoded = decrypt(crypted,len,u,n);
    printf("Decoded : %s\n",decoded);
    // Testing Init Keys
    Key* pKey = malloc(sizeof(Key));
    Key* sKey = malloc(sizeof(Key));
    init_pair_keys(pKey,sKey,3,7);
    printf("pKey: %lx, %lx \n", pKey->keyValue, pKey->N);
    printf("sKey: %lx, %lx \n", sKey->keyValue, sKey->N);

    // Testing Key Serialization
    char * chaine = key_to_str(pKey);
    printf("key_to_str : %s \n", chaine);
    Key* k = str_to_key(chaine);
    printf("str_to_key : %lx, %lx \n", k->keyValue, k->N);
    // Testing signature
    // Candidate Keys:
    Key* pKeyC = malloc(sizeof(Key));
    Key* sKeyC = malloc(sizeof(Key));
    init_pair_keys(pKeyC, sKeyC,3,7);
    // Declaration
    char *mess2 = key_to_str(pKeyC);
    char *ktstmp = key_to_str(pKey);
    printf("%s vote pour %s\n", ktstmp,mess2);
    Signature* sgn = sign(mess2,sKey);
    printf("Signature: ");
    print_long_vector(sgn->tab,sgn->tabSize);
    free(chaine);
    chaine = signature_to_str(sgn);
    printf("signature_to_str: %s \n", chaine);
    free(sgn->tab);
    free(sgn);
    sgn = str_to_signature(chaine);
    printf("str_to_signature: ");
    print_long_vector(sgn->tab,sgn->tabSize);
    // Testing protected
    Protected* pr = init_protected(pKey,mess2,sgn);
    // Verification
    if(verify(pr)) {
        printf("Signature valide \n");
    } else {
        printf("Signature non valide\n");
    }
    free(chaine);
    chaine = protected_to_str(pr);
    printf("protected_to_str : %s\n", chaine);
    free(pr);
    pr = str_to_protected(chaine);
    char *ktstr = key_to_str(pr->pKey);
    char *sgntostr = signature_to_str(pr->sign);
    printf("str_to_protected: %s %s %s\n",ktstr, pr->mess,sgntostr);

    // ----------- freeing allocated memory --------------
    freeSignature(sgn);
    free(crypted);
    free(decoded);
    free(chaine);
    freeProtected(pr);
    free(k);
    free(ktstmp);
    free(mess2);
    free(ktstr);
    free(sgntostr);
    free(pKey);
    free(sKey);
    free(pKeyC);
    free(sKeyC);

    // Generation aleatoire d'un processus de vote
    generate_random_data(100,10);
    // Affichage des cle publique des candidats
    CellKey* listKeys = read_public_keys("candidates.txt");
    print_list_keys(listKeys);
    delete_list_keys(listKeys);

    // Affichage et stockage d'une liste chainee de declarations signees
    FILE *fic = fopen("declarations.txt","r");
    CellProtected* LCP = read_protectedCell();
    keepValidCellProtected(LCP);
    printCellProtected(LCP);
    deleteListCellProtected(LCP);
    fclose(fic);
    return 0;
}



