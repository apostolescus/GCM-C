BYTE *multiplication_in_GF(BYTE *first_txt, BYTE *second_txt){
    
    BYTE cpy1[AES_BLOCK_SIZE]; //copy of first text
    BYTE cpy2[AES_BLOCK_SIZE]; //copy of second text
    BYTE R[AES_BLOCK_SIZE] = {0};

    R[15] = 225; // special polynome converted to decimal

    BYTE *result = (BYTE*)calloc(AES_BLOCK_SIZE, sizeof(BYTE));
    
    BYTE medium[AES_BLOCK_SIZE];
    BYTE first_bit[AES_BLOCK_SIZE];

    memcpy(cpy1, first_txt, 16);
    memcpy(cpy2, second_txt, 16);
    

    for(int i=0;i<127;i++){
        memcpy(medium,cpy2,16);
        medium >>i;

        if (medium & 1 == 1){

            for(int j=0; j<16; j++)
            {
                result[j]=result[j]^cpy1[j];
            }
        } 
        memcpy(first_bit,cpy1,16);
        first_bit >> 127;

        if (first_bit == 0){
            cpy1 >> 1;
        }
        else{
            cpy1 >> 1;
            for (int i=0;i<16;i++)
               cpy1[i]= cpy1[i] ^ R[i];
        }
    }

    return result;
}
