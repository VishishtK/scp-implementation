#include <iostream>
#include <fstream>
#include <string.h>
#include <openssl/evp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utils.h"
#include "crypto.h"

using namespace std;

void sendData(string ipAddress, unsigned char* cipherText, int cipherTextLen){
    int i=0;
    const char* temp = ipAddress.c_str();
    while(temp[i]!=':'){
        i++;
    }
    string ip = ipAddress.substr(0,i);
    string port = ipAddress.substr(i+1,ipAddress.length()-i);

    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(stoi(port));
    int address_length = sizeof(address);

    inet_pton(AF_INET, ip.c_str(),&address.sin_addr);

    connect(fd, (struct sockaddr*)&address,address_length);

    cout << "Sending encrypted data \n";

    int bytesSent=0;
    while(bytesSent<4){
        bytesSent = bytesSent + send(fd,&cipherTextLen,sizeof(int),0);
    }

    bytesSent=0;
    while(bytesSent<cipherTextLen){
        if(cipherTextLen-bytesSent>1024){
            bytesSent = bytesSent + send(fd,cipherText+bytesSent,1024,0);
        }else{
            bytesSent = bytesSent + send(fd,cipherText+bytesSent,cipherTextLen-bytesSent,0);
        }
    }
    return;
}

int main(int argc, char * argv[]){
    string filename = getFilename(argc, argv);
    string runningMode = getRunningMode(argc, argv);
    string ipAddress;
    if(runningMode.compare("remote")==0){
        ipAddress = getIPAddress(argc, argv);
    }

    string data = readFromFile(filename);
    const unsigned char* plainText = (const unsigned char*) data.c_str();
    int plainTextLen = data.length();

    string password;
    cout << "Password:";
    cin >> password;

    const EVP_CIPHER *aes256 = EVP_aes_256_gcm();

    const int keyLength = EVP_CIPHER_key_length(aes256);
    unsigned char* key = (unsigned char*)malloc(keyLength);
    genKey(password, key, aes256);

    const int ivLength = EVP_CIPHER_iv_length(aes256);
    unsigned char *iv = (unsigned char*)malloc(ivLength);
    genIV(iv,aes256);

    unsigned char* cipherText=(unsigned char*)malloc(ivLength+plainTextLen);
    memcpy(cipherText,iv,ivLength);

    int cipherTextLen;
    if(encrypt(plainText,plainTextLen,key,iv,aes256,cipherText+ivLength,&cipherTextLen)==0){
        return 0;
    }
    cout << "Successfully encrypted testfile ("<<(int)plainTextLen<<" bytes encrypted)\n";

    if(runningMode.compare("local")==0){
        if(writeToFile(filename,(char *)cipherText,plainTextLen)==33){
            return 33;
        }
        cout << "Successfully encrypted testfile to testfile.uf ("<<(int)plainTextLen<<" bytes written)\n";
    }else{
        sendData(ipAddress,(unsigned char*)cipherText,plainTextLen+ivLength);
        cout << "Successfully encrypted testfile and transmitted("<<(int)plainTextLen<<" bytes transmitted)\n";
    }

    for(int i=0;i<10;i++){
        cout<<hex<<(int)cipherText[i]<<" "<<dec;
    }
    cout << "(and all the rest...) \n";
}