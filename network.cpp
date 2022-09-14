#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
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

void recieveData(string port, unsigned char ** cipherText, int* cipherTextLen){
    // Creating socket and starting to listen for connections
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

    // Starting to read the bytes after accepting a connection
    // First read the first 4 bytes which tells us how many bytes of encrypted
    // data and IV we will recieve
    int bytesRead = 0;
    int totalBytes;
    while(bytesRead<4){
        bytesRead = bytesRead + read(socket, &totalBytes, 4);
    }

    // Read the IV and the encrypted data
    int totalBytesRead=0;
    *cipherText = (unsigned char *) malloc(totalBytes);
    *cipherTextLen = totalBytes;

    while(bytesRead!=0){
        bytesRead = recv(socket, *cipherText+totalBytesRead, totalBytes,0);
        totalBytesRead = totalBytesRead + bytesRead;
    }
    return;
}