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

// Remove this later and use std:: instead to improve performance
// Currently using namespace std brings in all components of the namespace, unneccessary
 
using namespace std::chrono;
using namespace std;



// int imgSizeX = 1025;
// int imgSizeY = 1025;
int imgSizeX = 1920;
int imgSizeY = 1152;
// int imgSizeX = 10;
// int imgSizeY = 10;

std::string binPath = "C:\\Users\\jerry\\OneDrive\\Documents\\Jerry\\ROLI_Lab\\VS_Code_Stuff\\Julia_Code\\BinaryFile.bin";

int imageIndex = -1; // determines whether image1 or image2 is saved to, starts at -1 so when iterated first time index%2 = 0
bool runSLMFunct = false;


char* triggerChar = new char[1];
int newTrigger = 0;
int oldTrigger = 0;
long long tstart;
long long tend;

// Class holds temporary image data for storing images before sending to SLM, allows for some delay to occur without conflict between image array read/write
class imageClass 
{    
  public:
    unsigned char* tempImage = new unsigned char[imgSizeX*imgSizeY];
};

imageClass imageClassArray[2]; // create 2 image objects to hold 2 images, one can be read while the other is written by another thread



// Just a function to run as a separate thread and verify image data has been copied
void loopFunction()
{
    while(true)
    {
        if((imageIndex >= 0) && (runSLMFunct))
        {
            runSLMFunct = false; // do this first to ensure change in index is tracked as fast as possible
            
            std::cout << "index " << imageIndex << ", loopFunct() imVal = " << (int) imageClassArray[imageIndex%2].tempImage[imgSizeX*imgSizeY-99]
                 << ", time microsec:\t" << (long long) duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() << "\n";
        }
    }
}


void ConvertBinaryToArrayWithTrigger(std::string fBinPath)
{
    // Open binary file paths (~300 microsec)
    std::ifstream imginfile(fBinPath,std::ifstream::binary);

    // Read trigger from first 16 bits of image bin (~300 microsec)
    // Initialize newTrigger and oldTrigger at beginning of function execution pre-loop
    imginfile.seekg (imgSizeX*imgSizeY,imginfile.end);
    imginfile.seekg (imgSizeX*imgSizeY);
    imginfile.read(triggerChar, 2); // read char up to 2 bytes
    newTrigger = 256*(int)triggerChar[0] + (int)triggerChar[1]; // convert 2 8-bit bytes into 1 16-bit int
    
    if(!imginfile.good()) {
        std::cout << "(1) Error occurred at trigger writing time!" << std::endl;
    }
    
    std::cout << "\nnewTrigger: " << newTrigger << ", oldTrigger: " << oldTrigger << "\n\n\n\n";
    oldTrigger = newTrigger;


    // Loop while looking into trigger bin file, if any change to trigger value is detected it causes image bin file to be read and stored
    while (true)
    {
        // Read trigger again (~300 microsec)
        imginfile.clear();
        imginfile.seekg (imgSizeX*imgSizeY,imginfile.end);
        imginfile.seekg (imgSizeX*imgSizeY);
        imginfile.read(triggerChar, 2); // read char up to 2 bytes
        newTrigger = 256*(int)triggerChar[0] + (int)triggerChar[1];

        if(!imginfile.good()) {
            std::cout << "(2) Error occurred at trigger writing time!" << std::endl;
        }


        // If Trigger has been sent, read new image data from bin and put into image array
        // Also check that imginfile is good (file is fully written and readable)
        if ((oldTrigger != newTrigger) && (imginfile.good()))
        {
            // Copy image data from binary file to myImage (~800-1000 microsec)
            imginfile.clear();
            imginfile.seekg (0,imginfile.end); // start from index 2 to avoid trigger bytes
            imginfile.seekg (0);

            imageIndex++;
            imginfile.read((char*) imageClassArray[imageIndex%2].tempImage, imgSizeX*imgSizeY);
            runSLMFunct = true; // causes other thread to run code in while() loop
            std::cout << "\nindex " << imageIndex << ", binReadFunct() time microsec:\t\t\t" << (long long) duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() << "\n"; 


            if(!imginfile.good()) {
                std::cout << "(3) Error occurred at image writing time!" << std::endl;
            }


            oldTrigger = newTrigger;
        }
    }
}





int main()
{
    std::cout << "\n\n\nStart of Main\n";

    // tstart = (long long) duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
    // //function1();
    // //function2();
    // tend = (long long) duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
    // cout << "\nTime without threading: " << (tend-tstart) << "\n";



    tstart = (long long) duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
    //thread (ConvertBinaryToArrayWithTrigger, binPath).detach();
    //thread (function2).detach();

    thread t1(ConvertBinaryToArrayWithTrigger, binPath);
    //thread t2(function2);
    thread t2(loopFunction);
    t1.join();
    t2.join();
    tend = (long long) duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
    std::cout << "\nTime with threading: " << (tend-tstart) << "\n";
    


    std::cout << "\nEnd of Main\n";
    return 0;
}

