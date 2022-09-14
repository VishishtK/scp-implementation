#include <openssl/evp.h>

int encrypt(const unsigned char *plainText, int len, unsigned char * key, unsigned char* IV, const EVP_CIPHER *aes256, unsigned char* cipherText, int* cipherTextLen);

void decrypt(const unsigned char *cipherText, int len, unsigned char * key, unsigned char* IV,const EVP_CIPHER *aes256, unsigned char* decryptedText, int* decryptedTextLen);