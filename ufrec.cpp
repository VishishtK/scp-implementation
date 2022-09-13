#include <iostream>
#include <fstream>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
using namespace std;

string getFilename(int argc, char * argv[]){
    if (argc <2)
    {
        cout << "Filename missing\n";
        exit(EXIT_FAILURE);
    }
    return argv[1];
}

string getRunningMode(int argc, char * argv[]){
    if(argc<3){
        // Default Mode
        cout << "Default (Local) running mode\n";
        return "local";
    }else{
            if(strcmp(argv[2],"-d")==0){
                cout << "Remote running mode\n";
                return "remote";
            }else{
                cout << "Local running mode\n";
                return "local";
            }
        }
}

string getPort(int argc, char * argv[]){
    if (argc<4)
    {
        cout << "Port missing for remote mode\n";
        exit(EXIT_FAILURE);
    }
    cout << "Port:"<<argv[3]<<"\n";
    return argv[3];
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

string readFromFile(string filename){
    ifstream myfile(filename);
    if (!myfile.is_open())
    {
        cout << "Unable to open file " << filename << "\n"; 
        exit(EXIT_FAILURE);
    }
    string fileInput((istreambuf_iterator<char>(myfile)),istreambuf_iterator<char>());
    return fileInput;
}

int writeToFile(string filename, char* data, int dataLen){
    string outputFileName = filename.substr(0,filename.length()-6);

    ifstream myfile(outputFileName);
    if (myfile.is_open())
    {
        cout << "Output file already exists, ABORTING " << outputFileName << "\n"; 
        return 33;
    }

    ofstream out(outputFileName);
    if(! out)
    {  
        cout<<"Cannot open output file\n";
        exit(EXIT_FAILURE);
    }
    out.write(data,dataLen);
    out.close();
    return 1;
}

void recieveData(string port, unsigned char ** cipherText, int* cipherTextLen){
    int fd;

    fd = socket(AF_INET,SOCK_STREAM,0);
    cout<<fd<<"\n";

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(stoi(port));
    int address_length = sizeof(address);

    bind(fd, (struct sockaddr*)&address,sizeof(address));

    cout << listen(fd,3)<<"\n";

    int socket = accept(fd,(struct sockaddr *)&address, (socklen_t*)&address_length);
    cout<<socket<<"\n";

    int bytesRead = 0;
    int totalBytes;
    while(bytesRead<4){
        bytesRead = bytesRead + read(socket, &totalBytes, 4);
    }

    bytesRead = 1;
    int totalBytesRead=0;
    unsigned char buffer[totalBytes];
    cout<< "Total Bytes:" << totalBytes<<"\n";
    cout << "Printing recieved data \n";

    while(bytesRead!=0){
        bytesRead = recv(socket, buffer+totalBytesRead, totalBytes,0);
        totalBytesRead = totalBytesRead + bytesRead;
    }
    *cipherText = buffer;
    *cipherTextLen = totalBytes;
    return;
}

int main(int argc, char * argv[])
{    
    string filename = getFilename(argc, argv);
    string runningMode = getRunningMode(argc, argv);
    string port;
    if(runningMode.compare("remote")==0){
        port = getPort(argc, argv);
    }

    unsigned char* cipherText=NULL;
    int cipherTextLen;
    string data;
    if(runningMode.compare("local")==0){
        data = readFromFile(filename);
        cipherText = (unsigned char*) data.c_str();
        cipherTextLen = data.length();
        
    }else{
        recieveData(port,&cipherText,&cipherTextLen);
        cout<<"DATA:" << cipherText<<"\n";
        cout<<"DATALEN:" << cipherTextLen<<"\n";
    }


    string password;
    cout << "Password:";
    cin >> password;
    cout << "\n";

    const unsigned char salt[] = "SodiumChloride";
    const int iterations = 4096;
    const EVP_CIPHER *aes256 = EVP_aes_256_gcm();
    const int keyLength = EVP_CIPHER_key_length(aes256);
    unsigned char key[keyLength];
    const int ivLength = EVP_CIPHER_iv_length(aes256);
    unsigned char iv[ivLength];


    if (PKCS5_PBKDF2_HMAC(password.c_str(),password.length(),salt,14,iterations,EVP_sha3_256(),keyLength,key)==0){
        cout<< "Unable to generate key";
        exit(EXIT_FAILURE);
    }
    cout << "KEY: ";
    for(int i=0;i<keyLength;i++){
        cout<<hex<<(int)key[i]<<" "<<dec;
    }
    cout << "\n";

    if (PKCS5_PBKDF2_HMAC((const char*)key,keyLength,salt,14,iterations,EVP_sha3_256(),ivLength,iv)==0){
        cout<< "Unable to generate iv";
        exit(EXIT_FAILURE);
    }
    cout << "IV: ";
    for(int i=0;i<ivLength;i++){
        cout<<hex<<(int)iv[i]<<" "<<dec;
    }
    cout << "\n";

    unsigned char* decryptedText=(unsigned char*)malloc(cipherTextLen);
    int decryptedTextLen;

    decrypt((const unsigned char*)cipherText,cipherTextLen,key,iv,aes256,decryptedText,&decryptedTextLen);
    cout<<decryptedText;
    return writeToFile(filename,(char *)decryptedText,decryptedTextLen);
}