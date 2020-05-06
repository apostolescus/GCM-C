#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/md5.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


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
    return i;
}
void hexa_printer(BYTE *print, int size, char *text){
    printf("%s :\n", text);
    for(int i=0;i<size;i++){
        printf("%x ", print[i]);
    }
}

BYTE* finish_crypt(int counter, BYTE *initial_infos, BYTE *hash){

    BYTE *auth_tag = (BYTE*)malloc(sizeof(BYTE)*16);
    BYTE total_size[AES_BLOCK_SIZE]={0};

    counter = counter*16;

    sprintf(total_size,"%d",counter);
    
    for(int i=0;i<16;i++){
        hash[i] = hash[i]^total_size[i];
    }
    for(int i=0;i<16;i++){
        auth_tag[i] = hash[i]^initial_infos[i];
    }

    return auth_tag;
}

void crypt_block(BYTE *iv, AES_KEY enckey, BYTE *text, BYTE *hash, BYTE *crypted_text){

    if ( iv[15] == 255){
        iv[14]++;
        iv[15] = 0;
    }
    else{
        iv[15]++;
    }

    AES_encrypt(iv,crypted_text,&enckey);

    for(int i=0;i<16;i++)
    {
        crypted_text[i] = crypted_text[i]^text[i];
    }
    for ( int i=0;i<16;i++){
        hash[i] = crypted_text[i]^hash[i];
    }
}

BYTE *crypt_plaintext(BYTE *iv, AES_KEY enckey, BYTE *hash, BYTE **auth_tag){
    
    char buffer[16],file_name[20];
    BYTE crypted[16];
    BYTE initial_infos[16];
    FILE *ptr, *read_file;
    BYTE *crypted_text=(BYTE*)malloc(sizeof(BYTE)*2048);//can be resized with calloc
    int i = 0, counter = 0, fsize;
    int rest = 0, padding;

    //crypt initial counter
    AES_encrypt(iv, initial_infos, &enckey);
     
    //get plaintext
    printf("introduceti numele fisierului: \n");

    while(1){
        scanf("%c", &file_name[i]);
        if(file_name[i] == '\n'){
            file_name[i] = '\0';
            break;
        }
        i++;
    }

    read_file = fopen(file_name,"rb");

    if( read_file == NULL){
        printf("file does not exist!\n");
        return NULL;
    }

    //get file size
    fseek(read_file,0,SEEK_END);
    fsize = ftell(read_file);
    fseek(read_file,0,SEEK_SET);

    rest = fsize % 16;
    counter = fsize / 16;

    for(int j=0; j < counter; j++){
        fread(buffer,16,1,read_file);
        crypt_block(iv,enckey,buffer,hash,crypted);
        memcpy(crypted_text+16*j,crypted,16);
    }

    if(rest != 0 ){
        
        fread(buffer,rest,1,read_file);
        padding = 15-rest;
        int content = rest;
        while(content<16){
            buffer[content] = padding;
            content++;
        }

        crypt_block(iv, enckey, buffer, hash, crypted);
        memcpy(crypted_text+16*counter,crypted,16);
        counter++;
    }

    fclose(read_file);
    // printf("introduceti textul in plain: \n");

    // while(1){
    //     scanf("%c",&buffer[i]);
    //     if(buffer[i] == '\n'){
    //         //jump to an end;
    //         if ( i != 0 ){
    //             rest = 15-i;
    //             printf("restul este: %d\n",rest);
    //             break;
    //         }
    //     }
    //     if( i == 15){
    //         crypt_block(iv,enckey,buffer,hash,crypted);
    //         memcpy(crypted_text+16*counter,crypted,16);
    //         i = 0;
    //         counter++;
    //     }
    //     else{
    //         i++;
    // }
    // }
    // //adding padding
    // if ( rest != 0 ){
    //     int j=15-rest;
    //     while(j<16){
    //         buffer[j] = rest;
    //         j++;
    //     }
 
    //     crypt_block(iv, enckey, buffer, hash, crypted);
    //     memcpy(crypted_text+16*counter,crypted,16);
    //     counter++;

    // }
   
    *auth_tag = finish_crypt(counter, initial_infos, hash);

    ptr = fopen("crypt.bin","wb");

    fwrite(crypted_text, counter*16,1,ptr);
    fclose(ptr);

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

bool calculate_auth_tag(BYTE *auth_tag, BYTE *hash, BYTE *crypted_text,BYTE *iv, AES_KEY enckey, int sz){
    
    FILE *fptr;
    BYTE total_size[AES_BLOCK_SIZE]={0};
    BYTE new_authtag[AES_BLOCK_SIZE];
    BYTE initial_infos[AES_BLOCK_SIZE];
    

    int counter = sz/16;
    AES_encrypt(iv,initial_infos,&enckey);
   
    for ( int j=0;j< counter; j++){
        for(int i=0;i<16;i++){
            hash[i] = hash[i]^crypted_text[j*16+i];
        }
    }

    sprintf(total_size,"%d",sz);
  
    for(int i=0;i<16;i++){
        hash[i] = hash[i]^total_size[i];
    }    
    

    for (int i=0; i<16;i++){
        new_authtag[i] = hash[i]^initial_infos[i];
    }

    for( int i=0;i<16;i++)
        if(new_authtag[i] != auth_tag[i])
            return false;
    
    return true;

}
bool decrypt_paintext(BYTE *iv, AES_KEY enckey,BYTE *hash, BYTE *crypted_text, BYTE *auth_tag){
    
    FILE *fptr;
    BYTE inter[AES_BLOCK_SIZE];
    BYTE enc_iv[AES_BLOCK_SIZE];
    char *text = (char*)malloc(sizeof(char)*2048);
    int sz;
    
    fptr = fopen("crypt.bin", "rb");
    fseek(fptr, 0, SEEK_END);
    sz = ftell(fptr);
    fclose(fptr);

    fptr = fopen("decrypt.txt","w+");

    if( calculate_auth_tag(auth_tag,hash, crypted_text,iv, enckey, sz) == false)
        return false;


    for (int j=0;j<sz/16;j++){
        if (iv[15]==255){
            iv[14]++;
            iv[15]=0;
        }
        else iv[15]++;

        AES_encrypt(iv,inter,&enckey);

        for(int i=0;i<16;i++){
            text[i+j*16] = inter[i]^crypted_text[i+j*16];
        }

    }
    
    
    int test = text[sz-1];//get padding value
    if (test < 16 && test > 0){ //remove padding
        text[sz-test-1] = '\0';
    }

    fwrite(text,sz-test-1,1,fptr);
    return true;
    
}   

int main(){

    FILE *ptr;
    int ad_info_length, counter;
    BYTE *userkey[AES_BLOCK_SIZE];
    BYTE hash[16], last_check[16], hash1[16], hash2[16], temp_key[16];
    BYTE *ad_infos, *crypted_text, *auth_tag, *decrypted_text, *cpy;
    BYTE *iv, iv_copy[16];

    AES_KEY enckey;
    
    RAND_bytes(userkey,AES_BLOCK_SIZE);

    AES_set_encrypt_key(userkey,AES_BLOCK_SIZE *8, &enckey);
    memcpy(temp_key,&enckey,16);

    ad_infos = (BYTE*)calloc(sizeof(BYTE),1024); //initial size of 1024 bytes can be resized with calloc
    cpy = ad_infos;
    ad_info_length = read_aditional_infos(ad_infos);

    iv = generate_random_iv(16);

    //encryption
    MD5(ad_infos,ad_info_length,hash);
    memcpy(hash1, hash, 16);
    memcpy(iv_copy,iv,16); //to keep the same iv for crypt and decrypt
  
    crypted_text = crypt_plaintext(iv_copy, enckey, hash1, &auth_tag);

    if (crypted_text == NULL)
        return 0;
    else{
        printf("data encrypted succesfully\n");
    }

    //decryption
    memcpy(iv_copy, iv, 16);
    ad_info_length = read_aditional_infos(cpy);
    MD5(cpy,ad_info_length,hash2);
    bool decrypt_text = decrypt_paintext(iv_copy, enckey, hash2, crypted_text, auth_tag);
   
    if ( decrypted_text == false){
        printf("wrong data\n");
    }
    else{
        printf("data decrypted succesfully\n");
    }

    return 0;
}
