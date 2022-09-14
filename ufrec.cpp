#include <iostream>
#include <fstream>
#include <string.h>
#include <openssl/evp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "utils.h"
#include "crypto.h"

using namespace std;

void recieveData(string port, unsigned char ** cipherText, int* cipherTextLen){
    int fd;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(stoi(port));
    int address_length = sizeof(address);

    fd = socket(AF_INET,SOCK_STREAM,0);
    bind(fd, (struct sockaddr*)&address,sizeof(address));
    listen(fd,3);
    int socket = accept(fd,(struct sockaddr *)&address, (socklen_t*)&address_length);

    int bytesRead = 0;
    int totalBytes;
    while(bytesRead<4){
        bytesRead = bytesRead + read(socket, &totalBytes, 4);
    }

    int totalBytesRead=0;
    *cipherText = (unsigned char *) malloc(totalBytes);
    *cipherTextLen = totalBytes;

    while(bytesRead!=0){
        bytesRead = recv(socket, *cipherText+totalBytesRead, totalBytes,0);
        totalBytesRead = totalBytesRead + bytesRead;
    }
    return;
}

int main(int argc, char * argv[]){    
    string filename = getFilename(argc, argv);
    string runningMode = getRunningMode(argc, argv);
    string port;
    if(runningMode.compare("remote")==0){
        port = getPort(argc, argv);
    }

    unsigned char* cipherText=NULL;
    int cipherTextLen;
    if(runningMode.compare("local")==0){
        string data = readFromFile(filename);
        cipherText = (unsigned char*) data.c_str();
        cipherTextLen = data.length();
        
    }else{
        recieveData(port,&cipherText,&cipherTextLen);
    }

    string password;
    cout << "Password:";
    cin >> password;

    const EVP_CIPHER *aes256 = EVP_aes_256_gcm();

    const int keyLength = EVP_CIPHER_key_length(aes256);
    unsigned char* key = (unsigned char*) malloc(keyLength);
    genKey(password, key, aes256);
    
    const int ivLength = EVP_CIPHER_iv_length(aes256);
    unsigned char *iv = (unsigned char*)malloc(ivLength);
    memcpy(iv,cipherText,ivLength);
    cout << "IV: ";
    for(int i=0;i<ivLength;i++){
        cout<<hex<<(int)iv[i]<<" "<<dec;
    }
    cout << "\n";

    cipherText = cipherText+ivLength;
    cipherTextLen = cipherTextLen - ivLength;

    unsigned char* decryptedText=(unsigned char*)malloc(cipherTextLen);
    int decryptedTextLen;
    decrypt((const unsigned char*)cipherText,cipherTextLen,key,iv,aes256,decryptedText,&decryptedTextLen);
    cout<<decryptedText<<"\n";
    return writeToFile(filename,(char *)decryptedText,decryptedTextLen);
}