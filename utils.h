#include <string.h>
using namespace std;

string getFilename(int argc, char * argv[]);

string getRunningMode(int argc, char * argv[]);

string getIPAddress(int argc, char * argv[]);

string getPort(int argc, char * argv[]);

void readFromFile(string filename, unsigned char** cipherText, int* cipherTextLen);

int writeToFile(string filename, char* data, int dataLen);

void printHex(unsigned char* data, int dataLen);