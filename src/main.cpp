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
#include <ctime>
#include <unistd.h>
#include <string>
#include <boost/filesystem.hpp>

using namespace std;

//Screen resolution-find better way to get this
#define HORIRES 1920
#define VERTRES 1080

int main(int argc, char* argv[])
{
	int numCams = 0; //number of connected cameras
	int camID = 0; //which camera is being read (0-5)
	int mode = 0; //Selects the functionality of the program
	Config Config1;	//decleare config class
	ASI_CAMERA_INFO CamInfo;
	IplImage* capture[6];
	char* directory;

	//check that save directory is valid
	if(argc == 2)
	{
        if (!boost::filesystem::is_directory(argv[1]))
        {
            cout << argv[1] <<" is no a valid directory\n";
            return -1;
        }
        else
            directory = argv[1];
	}
	else
        directory = 0;


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
		//ASISetControlValue(camID,ASI_HIGH_SPEED_MODE, 1, ASI_FALSE);

        ASIGetCameraProperty(&CamInfo, camID);
        capture[camID] = cvCreateImage(cvSize(CamInfo.MaxWidth,CamInfo.MaxHeight), 8, 1);
        //Set size to max resolution, 16=bit depth, 1=channels
	}

	while (mode != -1)
	{
		mode = modeSelectMenu();

        switch(mode){
            case -1:
                cout << "exiting....\n";
                break;
            case 1 :
                previewVideo(capture, numCams, Config1.Exposure);
                break;
            case 2 :
                takePhoto(capture, numCams, Config1.Exposure, CamInfo, directory);
                break;
            case 3 :
                recordDuration(capture, numCams, Config1.Exposure, CamInfo, directory);
                break;
            default :
                cout << "Invalid mode, please select again:\n";
        }
	}

	for(camID = 0; camID < numCams; camID++)
    {
        ASICloseCamera(camID); //This needs to be last
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

void previewVideo(IplImage* capture[6], int numCams, int exposure)
{
    char keepVid = 0;
    char cameraName[8];
    int camID = 0;

    for(camID = 0; camID < numCams; camID++)
        ASIStartVideoCapture(camID);


    cout << "Press ESC to exit video preview\n";

    while(keepVid!=27)
    {

        for(camID = 0; camID < numCams; camID++)
        {
            sprintf(cameraName, "Camera%d",camID);
            cvNamedWindow(cameraName, camID);
            cvResizeWindow(cameraName, HORIRES/3,VERTRES/2.5);
            //tile up to 6 windows on screen
            if(camID < 3)
                cvMoveWindow(cameraName, camID*HORIRES/3, 0);
            else
                cvMoveWindow(cameraName, (camID-3)*HORIRES/3, VERTRES/2);

            if(capture[camID]->depth == 16)
            {
               //needs to be scaled to 8 bit to be displayed corectly
                IplImage * scaledVid[6];
                scaledVid[camID] = cvCreateImage(cvGetSize(capture[camID]),8,1);
                cvConvertScale(capture[camID],scaledVid[camID],1.0/256) ;

                ASIGetVideoData(camID,(unsigned char*)scaledVid[camID]->imageData,scaledVid[camID]->imageSize,exposure);
                cvShowImage(cameraName, scaledVid[camID]);
                cvReleaseImage(&scaledVid[camID]);     //free the memory allocated to image capture
            }
            else
            {
                ASIGetVideoData(camID,(unsigned char*)capture[camID]->imageData,capture[camID]->imageSize,exposure);
                cvShowImage(cameraName, capture[camID]);
            }
        }
        keepVid = cvWaitKey(1);
    }
    //close all and free memory
    cvDestroyAllWindows();

    for(camID = 0; camID < numCams; camID++)
        ASIStopVideoCapture(camID);
}

int modeSelectMenu ()
{
    char modeRead;
    int mode = 0;

    cout << "select mode:\n";
    cout << "1\tVideo preview\n";
    cout << "2\tCapture image\n";
    cout << "3\tRecord video\n";
    cout << "\n0\tEXIT\n";
    cin >> modeRead;
    mode = modeRead - '0';

    if(mode == 0)
        mode = -1;

    return mode;
}

void takePhoto(IplImage* capture[6], int numCams, int exposure, ASI_CAMERA_INFO CamInfo, char* directory)
{
    int camID = 0; //loop this once working with one camera
    ASI_EXPOSURE_STATUS status[6];
    string photoName = "test"; // make this from time and sate
    string fileName;
    char tempCamID;
    long imgSize;

    if(capture[camID]->depth == 16)
        imgSize = CamInfo.MaxWidth*CamInfo.MaxHeight*2; //+1 for 16 bit
    else
        imgSize = CamInfo.MaxWidth*CamInfo.MaxHeight; //+1 for 16 bit

    if(directory != 0)
        photoName = directory + fileName;

    unsigned char* imgBuf[6];

    for(camID = 0; camID < numCams; camID++)
    {
        status[camID] = ASI_EXP_WORKING;
        imgBuf[camID] = new unsigned char[imgSize];
        ASIStartExposure(camID,ASI_TRUE);//starts exposure
    }

    int statusCheck  = 0;

    while(statusCheck < numCams)
    {
        for(camID = 0; camID < numCams; camID++)
        {
            ASIGetExpStatus(camID, &status[camID]);

            if(status[camID] != ASI_EXP_WORKING)
                statusCheck ++;
            if(statusCheck < numCams)
                statusCheck = 0;
        }
    }

    for(camID = 0; camID < numCams; camID++)
    {
        if(status[camID] == ASI_EXP_SUCCESS)
        {
            ASIGetDataAfterExp(camID,imgBuf[camID],imgSize);
            tempCamID = '0' + camID;
            fileName = photoName + "cam" + tempCamID + ".jpg";
            cout << fileName << endl;
            cvSaveImage(fileName.c_str(), capture[camID]);
            cout << "Good Capture camera " << camID << endl;
        }
        else
        {
            cout << "Failed to capture camera " << camID <<endl;
        }

        ASIStopExposure(camID);
        delete[] imgBuf[camID];
    }
}

void recordVideo(IplImage* capture[6], int numCams, int exposure, ASI_CAMERA_INFO CamInfo, int recTime, char* directory)
{
    CvVideoWriter* writer;

    int camID = 0;
    char keepVid = 0;
    string videoName = "testVid";
    string fileName;
    char tempCamID;

    //for(camID = 0; camID < numCams; camID++)
    ASIStartVideoCapture(camID);

    tempCamID = '0' + camID;
    fileName = videoName + "Cam" + tempCamID + ".avi";
    //set directory in file name if given
    if(directory != 0)
        fileName = directory + fileName;

    if(capture[camID]->depth == 16)
        writer = cvCreateVideoWriter(fileName.c_str(),CV_FOURCC('M','J','P','G'), 30,cvSize(CamInfo.MaxWidth,CamInfo.MaxHeight),0);
    else
        writer = cvCreateVideoWriter(fileName.c_str(),CV_FOURCC('M','J','P','G'), 60,cvSize(CamInfo.MaxWidth,CamInfo.MaxHeight),0);

    clock_t startTime;
    startTime = clock();

    cout << "recording " << recTime << " second video..please wait\n";

    while(recTime >= (clock()-startTime)/CLOCKS_PER_SEC)
    {

        if(capture[camID]->depth == 16)
        {
            IplImage * scaledVid[6];
            scaledVid[camID] = cvCreateImage(cvGetSize(capture[camID]),8,1);
            cvConvertScale(capture[camID],scaledVid[camID],1.0/256) ;

            ASIGetVideoData(camID,(unsigned char*)scaledVid[camID]->imageData,scaledVid[camID]->imageSize,exposure);
            cvWriteFrame(writer, scaledVid[camID]);
            cvReleaseImage(&scaledVid[camID]);     //free the memory allocated to image captu
        }
        else
        {
        ASIGetVideoData(camID,(unsigned char*)capture[camID]->imageData,capture[camID]->imageSize,exposure);
        cvWriteFrame(writer, capture[camID]);
        keepVid = cvWaitKey(1);

        }
    }
    cout << "Recording complete\n";
    cvReleaseVideoWriter(&writer);

    for(camID = 0; camID < numCams; camID++)
        ASIStopVideoCapture(camID);
}

void recordDuration(IplImage* capture[6], int numCams, int exposure, ASI_CAMERA_INFO CamInfo, char* directory)
{
    int duration = 0;
    cout << "Record duration:\n";
    cin >> duration;

    recordVideo(capture, numCams, exposure, CamInfo, duration, directory);
}
