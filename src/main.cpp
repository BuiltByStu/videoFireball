/*
File:		main.cpp
Authour:	Stuart Hahn
Version:	0.1
Date: 		29/03/18
Description:Sources configuration files and assesses connected cameras
*/

#include "stdio.h"
#include "ASICamera2.h"
#include "opencv2/core/core.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "main.h"
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[])
{
	int numCams = 0; //number of connected cameras
	int camID = 0; //which camera is being read (0-5)
	Config Config1;	//decleare config class
	ASI_CAMERA_INFO CamInfo;
	IplImage* capture[6];

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
	printConfig(Config1);

	//check status (and properties) of cameras
	for(camID = 0; camID < numCams; camID++)
	{
		if(cameraCheck(CamInfo, camID))
		{
			cout << "Unable to access camera" << camID << " check that permissions have been set for camera\n";
			return -1;
		}

		printCameraProperties(CamInfo, camID);	//print properties of the camera



		//set camera settings
		ASISetControlValue(camID,ASI_EXPOSURE, Config1.Exposure*1000, ASI_FALSE);
		//set exposure from config class, ASI_FALSE = not auto exposure
		ASISetControlValue(camID,ASI_GAIN,Config1.Gain, ASI_FALSE);
		(camID,ASI_BANDWIDTHOVERLOAD, 100, ASI_FALSE);
		//Allow 100% usb bandwidth
		//ASISetControlValue(camID,ASI_HIGH_SPEED_MODE, 0, ASI_FALSE);
		//Not sure if white balance (WB) needs to be set
		//ASISetControlValue(camID,ASI_WB_B, 90, ASI_FALSE);
	 	//ASISetControlValue(camID,ASI_WB_R, 48, ASI_TRUE);

        ASIGetCameraProperty(&CamInfo, camID);
        capture[camID] = cvCreateImage(cvSize(CamInfo.MaxWidth,CamInfo.MaxHeight), 16, 1);
        //Set size to max resolution, 16=bit depth, 1=channels

        ASIStartVideoCapture(camID);
	}

    //needs to be modified for multiple cameras
    cvNamedWindow("camera0", 0);

    int keepVid = 0;

    //Need to add ASIGetVideoData
    while(keepVid != 27)
    {
        keepVid = cvWaitKey(1);
        cout << keepVid;
        ASIGetVideoData(0,(unsigned char*)capture[0]->imageData,capture[0]->imageSize,Config1.Exposure);
        cvShowImage("camera0", capture[0]);
    }

    //close all and free memory
    cvDestroyAllWindows();

	for(camID = 0; camID < numCams; camID++)
    {
        ASIStopVideoCapture(camID);
        ASICloseCamera(camID);
        cvReleaseImage(&capture[camID]);     //free the memory allocated to image capture
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
		while(configFile_ >> camParam >> camVal && i<5)
		{
	 	   reading[i] = camVal;
			i++;
		}
		configFile_.close();

		Config1.Exposure = reading[0];
		Config1.Gain = reading[1];
		Config1.Gamma = reading[2];
		Config1.Bandwidth = reading [3];
		Config1.HS_Mode = reading [4];

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
	cout << "Camera configuration:\n";
	cout << "\tExposure (ms)=\t" << Config1.Exposure << endl;
	cout << "\tGain=\t\t" << Config1.Gain << endl;
	cout << "\tGamma=\t\t" << Config1.Gamma << endl;
	cout << "\tBancdwidth=\t" << Config1.Bandwidth << endl;
	cout << "\tHighSpeedMode=\t" << Config1.HS_Mode << endl;
}
