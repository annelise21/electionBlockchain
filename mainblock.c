#include "header/block.h"
#include <time.h>

int main() {
    FILE *f = fopen("temp.txt","r");
    FILE *fp = fopen("res.txt","w");
    FILE *ft = fopen("temps/pow.txt","w");

    // 
    Block *BlockTest = freadblock(f);
    fprintblock(fp,BlockTest);
    char *bts = block_to_str(BlockTest);
    printf("%s\n", bts);

    clock_t temps_initial;
    clock_t temps_final;
    double temps_cpu; 


    for(int i = 0; i < 2; i++) {
        temps_initial = clock();
        compute_proof_of_work(BlockTest,i);
        temps_final=clock();
        temps_cpu=((double)(temps_final-temps_initial))/CLOCKS_PER_SEC;
        if(verify_block(BlockTest,i)) {
            printf("Block vérifié\n");
        } else {
            printf("Block non vérifié\n");
        }
        fprintf(ft,"%d %f\n",i, temps_cpu);
    }
    fclose(f);
    fclose(fp);
    fclose(ft);
    free(bts);
    free(BlockTest->author);
    delete_block(BlockTest);
}