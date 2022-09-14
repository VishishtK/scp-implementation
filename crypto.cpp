#include <openssl/evp.h>
#include <iostream>

using namespace std;

int encrypt(const unsigned char *plainText, int len, unsigned char * key, unsigned char* IV, const EVP_CIPHER *aes256, unsigned char* cipherText, int* cipherTextLen){
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if(!EVP_CipherInit_ex(ctx,aes256,NULL,key,IV,1)){
        cout<<"Cipher to encrypt was not initialised\n";
        return 0;
    }

    if(!EVP_CipherUpdate(ctx,cipherText,cipherTextLen,plainText,len)){
        cout<<"Encryption failed\n";
        return 0;
    }

    EVP_EncryptFinal_ex(ctx,cipherText+ *cipherTextLen,cipherTextLen);
    return 1;
}

void decrypt(const unsigned char *cipherText, int len, unsigned char * key, unsigned char* IV,const EVP_CIPHER *aes256, unsigned char* decryptedText, int* decryptedTextLen){
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if(!EVP_CipherInit_ex(ctx,aes256,NULL,key,IV,0)){
        cout<<"Cipher to decrypt was not initialised\n";
        exit(EXIT_FAILURE);
    }
    if(!EVP_CipherUpdate(ctx,decryptedText,decryptedTextLen,cipherText,len)){
        cout<<"Decryption failed\n";
        exit(EXIT_FAILURE);
    }
}