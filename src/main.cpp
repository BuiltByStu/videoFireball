/*
File:		main.cpp
Authour:	Stuart Hahn
Version:	0.1
Date: 		29/03/18
Description:Sources configuration files and assesses connected cameras 
*/

#include "stdio.h"
#include "ASICamera2.h"
#include "main.h"
#include <iostream>
#include <fstream>

using namespace std;


int main(int argc, char* argv[])
{

	int numCams = 0;	//number of cameras connected
	Config Config1;	//decleare config class
	int reading[] = {0, 0, 0, 0, 0, 0, 0}; //int array for holding read from config file

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

		//Read from the configFile
	
		int i = 0;
		while(configFile_ >> camParam >> camVal && i<7)
		{
	 	   reading[i] = camVal;
			i++;
		}
		configFile_.close();

		Config1.Exposure = reading[0];
		Config1.Gain = reading[1];
		Config1.Gamma = reading[2];
		Config1.WB_R = reading [3];
		Config1.WB_B = reading [4];
		Config1.Bandwidth = reading [5];
		Config1.HS_Mode = reading [6];
	}
	
	else
		printf("Unable to read camConfig.txt\n");

	//print to check on the configfile	
	printConfig(Config1);



	return 0;
}


void printConfig(Config& Config1)
{
	cout << Config1.Exposure << endl;
	cout << Config1.Gain << endl;
	cout << Config1.Gamma << endl;
	cout << Config1.WB_R << endl;
	cout << Config1.WB_B << endl;
	cout << Config1.Bandwidth << endl;
	cout << Config1.HS_Mode << endl;
}
