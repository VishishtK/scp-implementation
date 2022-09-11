#include <iostream>
#include <fstream>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
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

int main(int argc, char * argv[])
{    
    string filename = getFilename(argc, argv);
    string runningMode = getRunningMode(argc, argv);
    string port;
    if(runningMode.compare("remote")==0){
        port = getPort(argc, argv);
    }

    string data = readFromFile(filename);
    const unsigned char* cipherText = (const unsigned char*) data.c_str();
    int cipherTextLen = data.length();

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
        cout<<hex<<(int)key[i]<<" ";
    }
    cout << "\n";

    if (PKCS5_PBKDF2_HMAC((const char*)key,keyLength,salt,14,iterations,EVP_sha3_256(),ivLength,iv)==0){
        cout<< "Unable to generate iv";
        exit(EXIT_FAILURE);
    }
    cout << "IV: ";
    for(int i=0;i<ivLength;i++){
        cout<<hex<<(int)iv[i]<<" ";
    }
    cout << "\n";

    unsigned char* decryptedText=(unsigned char*)malloc(cipherTextLen);
    int decryptedTextLen;

    decrypt((const unsigned char*)cipherText,cipherTextLen,key,iv,aes256,decryptedText,&decryptedTextLen);
    return writeToFile(filename,(char *)decryptedText,decryptedTextLen);
}