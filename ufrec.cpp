#include <iostream>
#include <string.h>
#include "utils.h"
#include "crypto.h"
#include "network.h"

using namespace std;


int main(int argc, char * argv[]){
    //Parsing the command line inputs
    string filename = getFilename(argc, argv);
    string runningMode = getRunningMode(argc, argv);
    string port;
    if(runningMode.compare("remote")==0){
        port = getPort(argc, argv);
    }

    // If mode is local then read the encrypted data to be decrypted from disk
    // Otherwise read the encrypted data from the socket.
    unsigned char* cipherText=NULL;
    int cipherTextLen;
    if(runningMode.compare("local")==0){
        string data = readFromFile(filename);
        cipherText = (unsigned char*) data.c_str();
        cipherTextLen = data.length();
        
    }else{
        recieveData(port,&cipherText,&cipherTextLen);
    }
    
    // Take password input
    string password;
    cout << "Password:";
    cin >> password;

    // Using AES 256 GCM cipher
    const EVP_CIPHER *aes256 = EVP_aes_256_gcm();

    // Genarting key from the password
    const int keyLength = EVP_CIPHER_key_length(aes256);
    unsigned char* key = (unsigned char*) malloc(keyLength);
    genKey(password, key, aes256);
    
    // The first ivLength bytes of the cipherText are IV which was added to 
    // the cipherText and sent over the nextwork
    const int ivLength = EVP_CIPHER_iv_length(aes256);
    unsigned char *iv = (unsigned char*)malloc(ivLength);
    memcpy(iv,cipherText,ivLength);
    cout << "IV: ";
    for(int i=0;i<ivLength;i++){
        cout<<hex<<(int)iv[i]<<" "<<dec;
    }
    cout << "\n";

    // After extracting the IV rest of the data is encrypted data
    cipherText = cipherText+ivLength;
    cipherTextLen = cipherTextLen - ivLength;

    unsigned char* decryptedText=(unsigned char*)malloc(cipherTextLen);
    int decryptedTextLen;
    
    // Decrypting the ciphertext
    decrypt((const unsigned char*)cipherText,cipherTextLen,key,iv,aes256,decryptedText,&decryptedTextLen);
    // cout<<decryptedText<<"\n";
    // Writing the decrypted data to disk
    return writeToFile(filename,(char *)decryptedText,decryptedTextLen);
}