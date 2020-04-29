#include <openssl/aes.h>
#include <openssl/rand.h>

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


void read_aditional_infos(BYTE *buffer){
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
}

int main(){

    BYTE *ad_infos;
    BYTE *iv;

    AES_KEY enckey;
    AES_KEY deckey;


    ad_infos = (BYTE*)malloc(sizeof(BYTE)*1024); //initial size of 1024 bytes
    read_aditional_infos(ad_infos);
    iv = generate_random_iv(16);

    for(int i=0;i<16;i++)
        printf("%d ",iv[i]);

    return 0;
}