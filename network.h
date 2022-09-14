#include <iostream>
#include <string.h>
using namespace std;

void sendData(string ipAddress, unsigned char* cipherText, int cipherTextLen);

void recieveData(string port, unsigned char ** cipherText, int* cipherTextLen);