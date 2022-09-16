#include <iostream>
#include <string.h>
#include "utils.h"
#include "crypto.h"
#include "network.h"

using namespace std;

int main(int argc, char * argv[]){
    // Parsing the command line inputs
    string filename = getFilename(argc, argv);
    string runningMode = getRunningMode(argc, argv);
    string ipAddress;
    if(runningMode.compare("remote")==0){
        ipAddress = getIPAddress(argc, argv);
    }

    // Reading data from the file which needs to be encrypted and stored/sent over the network
    unsigned char* plainText = NULL;
    int plainTextLen;
    readFromFile(filename,&plainText,&plainTextLen);
    

    // Take password input
    string password;
    cout << "Password:";
    cin >> password;

    // Using AES 256 GCM cipher
    const EVP_CIPHER *aes256 = EVP_aes_256_gcm();

    // Genarting key from the password
    const int keyLength = EVP_CIPHER_key_length(aes256);
    unsigned char* key = (unsigned char*)malloc(keyLength);
    genKey(password, key, aes256);

    // Genarating iv
    const int ivLength = EVP_CIPHER_iv_length(aes256);
    unsigned char *iv = (unsigned char*)malloc(ivLength);
    genIV(iv,aes256);

    // CipherText length = plain text lenght + length of IV since we are sending the IV also over the network by attaching
    // it to the start of the ciphertext
    unsigned char* cipherText=(unsigned char*)malloc(ivLength+plainTextLen);
    int cipherTextLen;
    // Attaching IV to the start of the cipher Text
    memcpy(cipherText,iv,ivLength);

    // Encrypt the plaintext
    if(encrypt(plainText,plainTextLen,key,iv,aes256,cipherText+ivLength,&cipherTextLen)==0){
        return 0;
    }
    cout << "Successfully encrypted "<<filename <<"("<<plainTextLen<<" bytes encrypted)\n";
    // cout<<"Encrypted file bytes :\n";
    // for(int i=ivLength;i<ivLength+plainTextLen;i++){
    //     cout<<hex<<(int)cipherText[i]<<" "<<dec;
    // }

    if(runningMode.compare("local")==0){
        if(writeToFile(filename+".ufsec",(char *)cipherText,plainTextLen+ivLength)==33){
            return 33;
        }
        cout << "Successfully written IV and CipherText to " <<filename+".ufsec " << "("<<plainTextLen+ivLength<<" bytes written)\n";
    }else{
        sendData(ipAddress,(unsigned char*)cipherText,plainTextLen+ivLength);
        cout << "Successfully transmitted IV and CipherText("<<plainTextLen+ivLength<<" bytes transmitted)\n";
    }
}