#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/md5.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef unsigned char BYTE;


BYTE* generate_random_iv(int size){

    BYTE *random_value = (BYTE*)malloc(sizeof(BYTE)*size);
    srand ((unsigned int) time (NULL));

    for (int i=0; i<size;i++){
        random_value[i] = rand();
    }
    return random_value;
}

int read_aditional_infos(BYTE *buffer){
    
    int i=0;
    printf("insert aditional infos: \n");
    
    while (1)
    {
        scanf("%c",&buffer[i]);
        if (buffer[i] == '\n'){
            buffer[i] = '\0';
            break;
        }
        i++;
        //realloc might be call if buffer is full
    }
    printf("returning i\n");
    return i;
}

BYTE* finish_crypt(int counter, BYTE *initial_infos, BYTE *hash){
    BYTE *auth_tag = (BYTE*)malloc(sizeof(BYTE)*16);

    BYTE total_size[AES_BLOCK_SIZE];
    sprintf(total_size,"%d",counter*16);

    for(int i=0;i<16;i++){
        hash[i] = hash[i]^total_size[i];
    }

    for(int i=0;i<16;i++){
        auth_tag[i] = hash[i]^initial_infos[i];
    }
    return auth_tag;
}

void crypt_block(BYTE *iv, AES_KEY enckey, BYTE *text, BYTE *hash, BYTE *crypted_text){

    iv[15]++;
    AES_encrypt(iv,crypted_text,&enckey);

    for(int i=0;i<16;i++)
    {
        crypted_text[i] = crypted_text[i]^text[i];
    }
    for ( int i=0;i<16;i++){
        hash[i] = crypted_text[i]^hash[i];
    }
}

BYTE *crypt_plaintext(BYTE *iv, AES_KEY enckey, BYTE *hash, BYTE *auth_tag, int leng){
    
    char buffer[16];
    BYTE crypted[16];
    BYTE initial_infos[16];

    BYTE *crypted_text=(BYTE*)malloc(sizeof(BYTE)*2048);//can be resized with calloc
    int i = 0, counter = 0;
    int rest = 0;

    //crypt initial counter
    AES_encrypt(iv, initial_infos, &enckey);

    //get plaintext
    printf("introduceti textul in plain: \n");

    while(1){
        scanf("%c",&buffer[i]);
        if(buffer[i] == '\n'){
            //jump to an end;
            if ( i != 0 ){
                rest = 15-i;
                printf("restul este: %d\n",rest);
                break;
            }
        }
        if( i == 15){
            crypt_block(iv,enckey,buffer,hash,crypted);
            memcpy(crypted_text+16*counter,crypted,16);
            i = 0;
            counter++;
        }
        i++;
    }
    
    if ( rest !=0 ){
        //aici este problema
        int j=15-rest;
        while(j<16){
            buffer[j] = rest;
            j++;
            printf("val lui j este: %d\n",j);
        }
        //ce este gresit la acest for?
        // for(int j=1; j++; j<16){
        //     printf("val lui j este: %d\n",j);
        //      buffer[j] = rest;
        // }
        crypt_block(iv, enckey, buffer, hash, crypted);
        memcpy(crypted_text+16*counter,crypted,16);
        counter++;

    }
    leng = counter;
    auth_tag = finish_crypt(counter, initial_infos, hash);

    return crypted_text;
}
void decrypt_block(BYTE *iv, AES_KEY enckey, char *text, BYTE *crypted_text){

    iv[15]++;
    BYTE interm[AES_BLOCK_SIZE];

    AES_encrypt(iv,interm, &enckey);
    for(int i=0;i<16;i++){
        text[i] = crypted_text[i]^interm[i];
    }
    
}
char *decrypt_paintext(BYTE *iv, int counter, AES_KEY enckey, BYTE *crypted_text){
    
    BYTE inter[AES_BLOCK_SIZE];
    BYTE enc_iv[AES_BLOCK_SIZE];
    char *text = (char*)malloc(sizeof(char)*2048);

    AES_encrypt(iv, enc_iv, &enckey);
    
    for(int k=0;k<counter;k++){
        decrypt_block(iv, enckey, text+counter*16, crypted_text+counter*16);
    }
    return text;
}   
int main(){

    int ad_info_length, counter;
    BYTE *userkey[AES_BLOCK_SIZE];
    BYTE hash[16], last_check[16];
    BYTE *ad_infos, *crypted_text, *auth_tag, *decrypted_text;
    BYTE *iv, iv_copy[16];

    AES_KEY enckey;
    
    RAND_bytes(userkey,AES_BLOCK_SIZE);

    AES_set_encrypt_key(userkey,AES_BLOCK_SIZE *8, &enckey);

    ad_infos = (BYTE*)malloc(sizeof(BYTE)*1024); //initial size of 1024 bytes
    ad_info_length = read_aditional_infos(ad_infos);

    iv = generate_random_iv(16);

    MD5(ad_infos,ad_info_length,hash);
    
    memcpy(iv_copy,iv,16); //to keep the same iv for crypt and decrypt
    crypted_text = crypt_plaintext(iv_copy, enckey, hash, auth_tag, counter);

    memcpy(iv_copy,iv,16);
    decrypted_text = decrypt_paintext(iv_copy, counter, enckey,crypted_text);

    for(int i=0;i<5;i++)
        printf("%s ",decrypted_text);
    printf("finally\n");
   

    return 0;
}








// BYTE *multiplication_in_GF(BYTE *first_txt, BYTE *second_txt){
    
//     BYTE cpy1[AES_BLOCK_SIZE]; //copy of first text
//     BYTE cpy2[AES_BLOCK_SIZE]; //copy of second text
//     BYTE R[AES_BLOCK_SIZE] = {0};

//     R[15] = 225; // special polynome converted to decimal

//     BYTE *result = (BYTE*)calloc(AES_BLOCK_SIZE, sizeof(BYTE));
    
//     BYTE medium[AES_BLOCK_SIZE];
//     BYTE first_bit[AES_BLOCK_SIZE];

//     memcpy(cpy1, first_txt, 16);
//     memcpy(cpy2, second_txt, 16);
    

//     for(int i=0;i<127;i++){
//         memcpy(medium,cpy2,16);
//         medium >>i;

//         if (medium & 1 == 1){

//             for(int j=0; j<16; j++)
//             {
//                 result[j]=result[j]^cpy1[j];
//             }
//         } 
//         memcpy(first_bit,cpy1,16);
//         first_bit >> 127;

//         if (first_bit == 0){
//             cpy1 >> 1;
//         }
//         else{
//             cpy1 >> 1;
//             for (int i=0;i<16;i++)
//                cpy1[i]= cpy1[i] ^ R[i];
//         }
//     }

//     return result;
// }
