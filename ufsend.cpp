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
    // Breaking down the address into IP and Port by splitting across ":"
    int i=0;
    const char* temp = ipAddress.c_str();
    while(temp[i]!=':'){
        i++;
    }
    string ip = ipAddress.substr(0,i);
    string port = ipAddress.substr(i+1,ipAddress.length()-i);

    // Creating socket;
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(stoi(port));
    int address_length = sizeof(address);

    inet_pton(AF_INET, ip.c_str(),&address.sin_addr);

    connect(fd, (struct sockaddr*)&address,address_length);

    cout << "Sending encrypted data \n";
    // Sending data over the socket
    // First 4bytes we send is the number of bytes we are going to send 
    int bytesSent=0;
    while(bytesSent<4){
        bytesSent = bytesSent + send(fd,&cipherTextLen,sizeof(int),0);
    }
    // Sending the actual encrypted data which has the IV appended at the begining
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
    // Parsing the command line inputs
    string filename = getFilename(argc, argv);
    string runningMode = getRunningMode(argc, argv);
    string ipAddress;
    if(runningMode.compare("remote")==0){
        ipAddress = getIPAddress(argc, argv);
    }

    // Reading data from the file which needs to be encrypted and stored/sent over the network
    string data = readFromFile(filename);
    const unsigned char* plainText = (const unsigned char*) data.c_str();
    int plainTextLen = data.length();

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

    if(runningMode.compare("local")==0){
        if(writeToFile(filename,(char *)cipherText,plainTextLen)==33){
            return 33;
        }
        cout << "Successfully written to disk ("<<plainTextLen+ivLength<<" bytes written)\n";
    }else{
        sendData(ipAddress,(unsigned char*)cipherText,plainTextLen+ivLength);
        cout << "Successfully transmitted("<<plainTextLen+ivLength<<" bytes transmitted)\n";
    }

    for(int i=0;i<10;i++){
        cout<<hex<<(int)cipherText[i]<<" "<<dec;
    }
    cout << "(and all the rest...) \n";
}