#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include <string>
#include <cstring>
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include <sstream>

using namespace std;

struct emp{
    int eid;
    string ename;
    int age; 
    double salary;
};

int main() {
    
    ifstream fs;
    fs.open("employee.csv");

    fs.seekg(0, fs.end);
    int length = fs.tellg();
    fs.seekg(0,fs.beg);
    cout << length << "\n";
    
    char *buff = new char[length];
    string s[2000];
    fs.read (buff,length);

    // cout << buff << "\n";
    // char emp[1000][1000];
    // int i = 0;
    // int row = 0;
    // while (i < length){
    //     //cout << buff[i] << "\n";
    //     if (buff[i] == '\n'){
    //         i++;
    //         row++;
    //     }
    //     emp[row][i] = buff[i];
    //     i++;
        
    // }
    // cout << *emp[1];
    
    istringstream buffer( buff);
    string token;
    int i=0;
    // while( getline( buffer, token, '\n' ) )
    // {
    //     s[i] = token;
    //     i++;

    // }
    // i=0;
   while( getline( buffer, token, '\n' ) )
    {
        s[i] = token;
        i++;

    }
   //cout << s[3] << endl;
    emp e[10];

    string b;
    e[3] = {10, "james", 23, 20000};

    cout << e[3].ename << endl;

    return 0;


}

