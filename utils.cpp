#include <iostream>
#include <fstream>
#include <string.h>
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


string getPort(int argc, char * argv[]){
    if (argc<4)
    {
        cout << "Port missing for remote mode\n";
        exit(EXIT_FAILURE);
    }
    cout << "Port:"<<argv[3]<<"\n";
    return argv[3];
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

int writeToFile(string outputFileName, char* data, int dataLen){
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

void printHex(unsigned char* data, int dataLen){
    for(int i=0;i<dataLen;i++){
        cout<<hex<<(int)data[i]<<" "<<dec;
    }
    cout << "\n";
}