
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <array>
#include <windows.h> // for Sleep()
#include <ctime>
#include <chrono>
#include <cstdint>
#include <iomanip> // for displaying double with high precision
#include <thread>
#include <algorithm> // for copy() array
#include <iterator> // for copy() array
#include <vector>

// Remove this later and use std:: instead to improve performance
// Currently using namespace std brings in all components of the namespace, unneccessary
 
using namespace std::chrono;
using namespace std;

// int imgSizeX = 1025;
// int imgSizeY = 1025;
int imgSizeX = 1920;
int imgSizeY = 1152;
// int imgSizeX = 30;
// int imgSizeY = 30;

unsigned char* myImage = new unsigned char[(imgSizeX*imgSizeY) + 2];
int triggerNumber = 0;


std::string binPath = "C:\\Users\\jerry\\OneDrive\\Documents\\Jerry\\ROLI_Lab\\VS_Code_Stuff\\Julia_Code\\BinaryFile.bin";
long long tstart;
long long tend;



int main()
{
    std::ofstream outfile(binPath,std::ofstream::binary);
    
    string myAnswer;
    cout << "\nWrite to .bin? (Y/N): ";
    cin >> myAnswer;

    std::fill_n(myImage, imgSizeX*imgSizeY, 1);

    if (myAnswer == "Y")
    {
        for (int i = 0; i < 5; i++)
        {
            triggerNumber = triggerNumber + 1;
            tstart = (long long) duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
            myImage[imgSizeX*imgSizeY-99] = 100+i; // Quickly change array at distant index to check if bin is written and received properly
            
            UINT8 lowerByte = triggerNumber%256;
            UINT8 upperByte = (triggerNumber-lowerByte)/256;

            cout << "\nupperByte: " << (int) upperByte << ",  lowerByte: " << (int) lowerByte << ", triggerNumber = " << triggerNumber;

            myImage[imgSizeX*imgSizeY] = upperByte;
            myImage[imgSizeX*imgSizeY+1] = lowerByte;

            outfile.clear();
            outfile.seekp(0);
            outfile.write((const char*)myImage, imgSizeX*imgSizeY+2);

            tend = (long long) duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

            cout << "\ni: " << i << ",  Image written to bin file, val = " << (int) myImage[imgSizeX*imgSizeY-99];
            cout << "\nTime for image bin read, microsec: " << (tend-tstart) << "\n\n";
        }
    }

    outfile.close();

    cout << "\nEnd of Main\n\n";
    return 0;
}