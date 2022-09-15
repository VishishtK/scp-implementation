#include <openssl/evp.h>
#include <iostream>
#include <string.h>
#include "utils.h"
#include <openssl/rand.h>

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

void genKey(string password, unsigned char * key, const EVP_CIPHER *aes256){
    const unsigned char salt[] = "SodiumChloride";
    const int iterations = 4096;
    const int keyLength = EVP_CIPHER_key_length(aes256);

    if (PKCS5_PBKDF2_HMAC(password.c_str(),password.length(),salt,14,iterations,EVP_sha3_256(),keyLength,key)==0){
        cout<< "Unable to generate key";
        exit(EXIT_FAILURE);
    }

    cout << "KEY: ";
    printHex(key,keyLength);

    return;
}

void genIV(unsigned char * iv,const EVP_CIPHER *aes256){

    const int ivLength = EVP_CIPHER_iv_length(aes256);
    int rc = RAND_bytes(iv,ivLength);

    if(rc!=1){
        cout << "IV gen failed \n";
        exit(EXIT_FAILURE);
    }

    cout << "IV: ";
    printHex(iv,ivLength);

    return;

}