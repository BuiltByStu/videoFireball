/*
File:		main.cpp
Authour:	Stuart Hahn
Version:	0.1
Date: 		29/03/18
Description:	Sources configuration files and assesses connected cameras 

*/

#include "stdio.h"
#include "ASICamera2.h"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{

    int numCams = 0;    //number of cameras connected

    printf("Checking camera connections\n");
    //for testing, can be removed later

    numCams = ASIGetNumOfConnectedCameras();
    //check the number of connected cameras

    if (numCams == 1)
        printf("1 camera has been detected\n");
    else
        printf("%d cameras have been detected\n",numCams);
    //prints the number of cameras which have been detected

    string configLine_;
    ifstream configFile_ ("camConfig.txt");

    if(configFile_.is_open())
    {
        printf("successfully opened camConfig.txt\n");

	string camParam;
	int camVal;
	

        while(configFile_ >> camParam >> camVal)
        {
            cout << camVal << "<>" << endl;
        }
        configFile_.close();
    }
    else
        printf("Unable to read camConfig.txt\n");


    return 0;
}
