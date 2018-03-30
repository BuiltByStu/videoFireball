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
	int numCams = 0; //number of connected cameras
	int selectCam = 0; //which camera is being read (0-5)
	int cameraCheck = 0; //Check that the camera can be read from
	Config Config1;	//decleare config class
	ASI_CAMERA_INFO CamInfo;

	numCams = cameraDetect();
	if(!numCams)
	{
		cout << "Please connect camera and re-run\n";
		return -1;
	}

	if(!readConfiguration(Config1))
	{
		cout << "Unable to read camConfig.txt\n";
		return -1;
	}

	//print to check on the configfile	
	//printConfig(Config1);

	//check status of cameras
	for(int i = 0; i < numCams; i++)
	{
		if(cameraCheck)
		{
			cout << "Unable to access camera" << i << " check that permissions have been set for camera\n";
			return -1;
		}

		printCameraProperties(CamInfo, i);	//print properties of the camera

	}

	cout << "Ran successfully to completion!\n";
	return 0;
}


int cameraDetect()
{
	int numCams = 0;	//number of cameras connected
	
	printf("Checking camera connections\n");
	//for testing, can be removed later

	numCams = ASIGetNumOfConnectedCameras();
	//check the number of connected cameras

	if (numCams == 1)
		printf("1 camera has been detected\n");

	else
		printf("%d cameras have been detected\n",numCams);
		//prints the number of cameras which have been detected

	return numCams;
}

int readConfiguration(Config& Config1)
{
	int reading[] = {0, 0, 0, 0, 0, 0, 0}; //int array for holding read from config file
	int readCheck = 0;

	string configLine_;
	ifstream configFile_ ("camConfig.txt");

	if(configFile_.is_open())
	{
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

		readCheck = 1;
	}
	

	return readCheck;
}

int cameraCheck (ASI_CAMERA_INFO CamInfo, int cameraID)
{
	int cameraCheck = 0;	//0 if all ok

	ASIGetCameraProperty(&CamInfo, cameraID);
	cameraCheck = ASIOpenCamera(CamInfo.CameraID);	//check that cameras can be opened
	cameraCheck += ASIInitCamera(CamInfo.CameraID);  //check that cameras can be initialised

	return cameraCheck;
}

void printCameraProperties(ASI_CAMERA_INFO CamInfo, int cameraID)
{
	ASIGetCameraProperty(&CamInfo, cameraID);
	cout << "Camera\t" << CamInfo.CameraID <<endl;
	cout << "\tCamera Model:\t" << CamInfo.Name << endl;
	cout << "\tResolution:\t" << CamInfo.MaxWidth << "x" << CamInfo.MaxHeight << endl;
	
	cout << "\tColour type:\t";
	if(!CamInfo.IsColorCam)
		cout << "mono\n";		
	else
		cout <<	"Colour\n";
	
	cout << "\tPixel Size:\t" << CamInfo.PixelSize << "um\n";
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
