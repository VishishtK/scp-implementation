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

string getIPAddress(int argc, char * argv[]){
    if (argc<4)
    {
        cout << "IP address missing for remote mode\n";
        exit(EXIT_FAILURE);
    }
    cout << "IP:"<<argv[3]<<"\n";
    return argv[3];
}

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
    string fileExtension = ".ufsec";
    string outputFileName = filename +fileExtension;

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
    string ipAddress;
    if(runningMode.compare("remote")==0){
        string ipAddress = getIPAddress(argc, argv);
    }

    string data = readFromFile(filename);
    const unsigned char* plainText = (const unsigned char*) data.c_str();
    int plainTextLen = data.length();

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

    unsigned char* cipherText=(unsigned char*)malloc(plainTextLen);
    int cipherTextLen;
    if(encrypt(plainText,plainTextLen,key,iv,aes256,cipherText,&cipherTextLen)==0){
        return 0;
    }
    if(runningMode.compare("local")==0){
        if(writeToFile(filename,(char *)cipherText,plainTextLen)==33){
            return 33;
        }
    }

    cout << "Successfully encrypted testfile to testfile.uf ("<<(int)plainTextLen<<" bytes written)\n";
    for(int i=0;i<10;i++){
        cout<<hex<<(int)cipherText[i]<<" ";
    }
    cout << "(and all the rest...) \n";
}