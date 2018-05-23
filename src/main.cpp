/*
File:		main.cpp
Authour:	Stuart Hahn
Version:	0.1
Date: 		23/05/18
Description:Main functions for videoFireball program
*/

#include <stdio.h>
#include <stdlib.h>
#include "ASICamera2.h"
#include "opencv2/core/core.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include "main.hpp"
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <ctime>
#include <unistd.h>
#include <string>
#include <boost/filesystem.hpp>


using namespace std;

//Screen resolution
#define HORIRES 1920
#define VERTRES 1080

int main(int argc, char* argv[])
{
	int numCams = 0; //number of connected cameras
	int camID = 0; //which camera is being read (0-5)
	Config Config1;	//decleare config class
	int autoMode = 0;
	ASI_CAMERA_INFO CamInfo[6];
	IplImage* capture[6];
	char* directory;
	char noDir[] = "./";

	//check that save directory is valid
	if(argc >= 2)
	{
        if(strcmp(argv[1],"auto")==0)
            autoMode = 1;
        else if(strcmp(argv[1],"help")==0)
        {
            help();
            return -1;
        }
        else if (!boost::filesystem::is_directory(argv[1]))
        {
            cout << argv[1] <<" is not a valid directory.\n";
            cout << "Enter \"./videofireball help\" for more information.\n";
            return -1;
        }
        else
            directory = argv[1];
	}
	else
        directory = noDir;

    if(argc == 3)
    {
        if(strcmp(argv[2],"auto")==0)
            autoMode = 1;
    }

	numCams = cameraDetect();
	if(!numCams)
	{
		cout << "Please connect camera and re-run\n";
		return -1;
	}

	//check status (and properties) of cameras
	for(camID = 0; camID < numCams; camID++)
	{
		if(cameraCheck(CamInfo[camID], camID))
		{
			cout << "Unable to access camera" << camID << " check that permissions have been set for camera\n";
			return -1;
		}

		printCameraProperties(CamInfo[camID], camID);	//print properties of the camera
        ASIGetCameraProperty(&CamInfo[camID], camID);
        capture[camID] = cvCreateImage(cvSize(CamInfo[camID].MaxWidth,CamInfo[camID].MaxHeight), 8, 1);
        //Set size to max resolution, 8=bit depth, 1=channels
	}

    if(!readConfiguration(Config1, numCams))
	{
		cout << "Unable to read camConfig.vfconf\n";
		return -1;
	}

    //print to check on the configfile
	printConfig(Config1);

	if(!autoMode)
        modeSelectMenu(capture, numCams, Config1, CamInfo, directory);
    else
        autoVideo(capture, numCams, Config1, CamInfo, directory);

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

int readConfiguration(Config& Config1, int numCams)
{
	int reading[] = {0, 0, 0, 0, 0, 0}; //int array for holding read from config file
	int readCheck = 0;
	int camID = 0;

	string configLine_;
	ifstream configFile_ ("camConfig.vfconf");

	if(configFile_.is_open())
	{
		string camParam;
		int camVal;

		//Read from the configFile

		int i = 0;
		while(configFile_ >> camParam >> camVal && i<6)
		{
	 	   reading[i] = camVal;
			i++;
		}
		configFile_.close();

		Config1.Exposure = reading[0];
		Config1.Gain = reading[1];
		Config1.Gamma = reading[2];
		Config1.Interval = reading [3];
		Config1.Iterations = reading [4];
		Config1.VideoDuration = reading [5];

		readCheck = 1;
	}
    for(camID = 0; camID < numCams; camID++)
    {
        //set camera settings
		ASISetControlValue(camID,ASI_EXPOSURE, Config1.Exposure*1000, ASI_FALSE);
		//set exposure from config class, ASI_FALSE = not auto exposure
		ASISetControlValue(camID,ASI_GAIN,Config1.Gain, ASI_FALSE);
		ASISetControlValue(camID,ASI_BANDWIDTHOVERLOAD, 90, ASI_FALSE);
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
	cout << "\tInterval(s)=\t" << Config1.Interval << endl;
	cout << "\tIterations=\t" << Config1.Iterations << endl;
	cout << "\tVid lentgh(s)=\t" << Config1.VideoDuration << endl;
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
    cvDestroyAllWindows();     //close all and free memory

    for(camID = 0; camID < numCams; camID++)
        ASIStopVideoCapture(camID);
}

void modeSelectMenu (IplImage* capture[6], int numCams, Config Config1, ASI_CAMERA_INFO CamInfo[6], char* directory)
{
    char modeRead;
    int mode = 0; //Selects the functionality of the program

	while (mode != -1)
	{
        cout << "select mode:\n";
        cout << "1\tVideo preview\n";
        cout << "2\tCapture image\n";
        cout << "3\tRecord video\n";
        cout << "4\tTimed capture\n";
        cout << "5\tUpdate Config \n";
        cout << "6\tGain testing\n";
        cout << "7\tEvent detection\n";
        cout << "\n0\tEXIT\n";
        cin >> modeRead;
        mode = modeRead - '0';

        if(mode == 0)
            mode = -1;

        switch(mode){
            case -1:
                cout << "exiting....\n";
                break;
            case 1 :
                previewVideo(capture, numCams, Config1.Exposure);
                break;
            case 2 :
                takePhoto(numCams, Config1.Exposure, CamInfo, directory);
                break;
            case 3 :
                recordDuration(capture, numCams, Config1.Exposure, CamInfo, directory);
                break;
            case 4 :
                timedCapture(capture, numCams, Config1, CamInfo, directory);
                break;
            case 5 :
                if(!readConfiguration(Config1,numCams))
                    cout << "Unable to read camConfig.vfconf\n";
                printConfig(Config1);
                break;
            case 6 :
                gainTest(numCams, Config1, CamInfo, directory);
                break;
            case 7 :
                eventDetect(capture, numCams, Config1.Exposure, CamInfo, Config1.VideoDuration, directory);
                break;
            default :
                cout << "Invalid mode, please select again:\n";
        }
	}
}

void takePhoto(int numCams, int exposure, ASI_CAMERA_INFO CamInfo[6], char* directory)
{
    int camID = 0; //loop this once working with one camera
    ASI_EXPOSURE_STATUS status[numCams];
    IplImage* photos[numCams];
    string photoName;
    string fileName;
    char tempCamID;
    long imgSize;


    imgSize = CamInfo[0].MaxWidth*CamInfo[0].MaxHeight*2; //+1 for 16 bit

    photoName = directory + timeStamp();

    for(camID = 0; camID < numCams; camID++)
    {
        photos[camID] = cvCreateImage(cvSize(CamInfo[camID].MaxWidth,CamInfo[camID].MaxHeight),8,1);
        status[camID] = ASI_EXP_WORKING;
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
            ASIGetDataAfterExp(camID,(unsigned char*)photos[camID]->imageData,imgSize);
            tempCamID = '0' + camID;

            fileName = photoName + "cam" + tempCamID +".jpg";
            cvSaveImage(fileName.c_str(), photos[camID]);
            cout << "Good Capture camera " << camID << endl;
        }
        else
        {
            ASIStopExposure(camID);
            cout << "Failed to capture camera " << camID <<endl;
        }
        cvReleaseImage(&photos[camID]);     //free the memory allocated to image capture
    }
}

void recordVideo(IplImage* capture[6], int numCams, int exposure, ASI_CAMERA_INFO CamInfo[6], int recTime, char* directory)
{
    CvVideoWriter* writer[numCams];

    int camID = 0;
    char keepVid = 0;
    string videoName;
    string fileName;
    char tempCamID;
    clock_t startTime;

    videoName = directory + timeStamp();

    for(camID = 0; camID < numCams; camID++)
    {
        ASIStartVideoCapture(camID);

        //make name for file
        tempCamID = '0' + camID;
        fileName = videoName + "cam" + tempCamID + ".avi";

        if(capture[camID]->depth == 16)
            writer[camID] = cvCreateVideoWriter(fileName.c_str(),CV_FOURCC('M','J','P','G'), 30,cvSize(CamInfo[camID].MaxWidth,CamInfo[camID].MaxHeight),0);
        else
            writer[camID] = cvCreateVideoWriter(fileName.c_str(),CV_FOURCC('M','J','P','G'), 50,cvSize(CamInfo[camID].MaxWidth,CamInfo[camID].MaxHeight),0);
    }

    cout << "recording " << recTime << " second video..please wait\n";

    startTime = clock();    //set start time

    while(recTime >= (clock()-startTime)/CLOCKS_PER_SEC)
    {
        for(camID = 0; camID < numCams; camID++)
        {
            if(capture[camID]->depth == 16)
            {
                IplImage * scaledVid;
                scaledVid = cvCreateImage(cvGetSize(capture[camID]),8,1);
                cvConvertScale(capture[camID],scaledVid,1.0/256) ;

                ASIGetVideoData(camID,(unsigned char*)scaledVid->imageData,scaledVid->imageSize,exposure);
                cvWriteFrame(writer[camID], scaledVid);
                cvReleaseImage(&scaledVid);     //free the memory allocated to image captu
            }
            else
            {
            ASIGetVideoData(camID,(unsigned char*)capture[camID]->imageData,capture[camID]->imageSize,exposure);
            cvWriteFrame(writer[camID], capture[camID]);
            keepVid = cvWaitKey(1);
            }
        }
    }
    cout << "Recording complete\n";

    for(camID = 0; camID < numCams; camID++)
    {
        cvReleaseVideoWriter(&writer[camID]);
        ASIStopVideoCapture(camID);
    }

}

void recordDuration(IplImage* capture[6], int numCams, int exposure, ASI_CAMERA_INFO CamInfo[6], char* directory)
{
    int duration = 0;

    cout << "Enter 0 to exit\nRecord duration:\n";
    cin >> duration;

    if(duration)
        recordVideo(capture, numCams, exposure, CamInfo, duration, directory);
}

std::string timeStamp()
{
    //This works but can be minimised
    string currentDateTime;
    string year, month, day, hour, mins, secs;

    time_t setTime = time(0);
    tm* getTime = localtime(&setTime);

    year = to_string(getTime->tm_year + 1900);

    if(getTime->tm_mon+1 < 10)
        month = '0' + to_string(getTime->tm_mon+1);
    else
        month = to_string(getTime->tm_mon+1);

    if(getTime->tm_mday < 10)
        day = '0' + to_string(getTime->tm_mday);
    else
        day = to_string(getTime->tm_mday);

    if(getTime->tm_hour < 10)
        hour = '0' + to_string(getTime->tm_hour);
    else
        hour = to_string(getTime->tm_hour);

    if(getTime->tm_min < 10)
        mins = '0' + to_string(getTime->tm_min);
    else
        mins = to_string(getTime->tm_min);

    if(getTime->tm_sec < 10)
        secs = '0' + to_string(getTime->tm_sec);
    else
        secs = to_string(getTime->tm_sec);

    currentDateTime = year + month + day + hour + mins + secs;

    return currentDateTime;
}

void timedCapture(IplImage* capture[6], int numCams, Config Config1, ASI_CAMERA_INFO CamInfo[6], char* directory)
{
    int mode = -1; //1=video,2=photo
    int interval = 0;
    int iterations = 0;
    int duration = 0;
    int startTime = 0;
    while(mode < 0 || mode > 2)
    {
        cout << "Select capture type:\n";
        cout << "1\tVideo\n2\tPhoto\n";
        cout << "\n0\tEXIT\n";
        cin >> mode;
    }

    while(interval == 0 && mode)
    {
        cout << "Enter 0 to exit\nEnter how often would you like captures to occur(seconds):\n";
        cin >> interval;
        if(interval == 0)
            mode = 0;
    }

    while(iterations == 0 && mode)
    {
        cout << "Enter 0 to exit\nEnter number of iterations:\n";
        cin >> iterations;
        if (iterations == 0)
            mode = 0;
    }

    if(mode == 1)
    {
        while(!duration && mode)
        {
            cout << "Enter 0 to exit\nRecord duration(seconds):\n";
            cin >> duration;

            if(duration == 0)
                mode = 0;

            if(duration && duration >= interval+1)
            {
                cout << "Record duration must be less than interval between captures\n";
                duration = 0;
            }
        }
    }

    if(mode)
    {
        Config1.Interval = interval;
        Config1.Iterations = iterations;
        Config1.VideoDuration = duration;

        switch(mode){
        case 1 :
            autoVideo(capture, numCams, Config1, CamInfo, directory);
            break;
        case 2 :
            autoPhoto(numCams, Config1, CamInfo, directory);
            break;
        }
    }
}

void autoVideo(IplImage* capture[6], int numCams, Config Config1, ASI_CAMERA_INFO CamInfo[6], char* directory)
{
    clock_t startTime;

    cout << "Auto video mode\n";
    for(int i = 0; i < Config1.Iterations; i++)
    {
        cout << "Starting capture\n";
        recordVideo(capture, numCams, Config1.Exposure, CamInfo, Config1.VideoDuration, directory);
        cout << "Waiting until next capture\n";

        startTime = clock();
        if (i <= Config1.Iterations-1)
            while(Config1.Interval-Config1.VideoDuration-1 >= (clock()-startTime)/CLOCKS_PER_SEC){}
    }
}

void autoPhoto(int numCams, Config Config1, ASI_CAMERA_INFO CamInfo[6], char* directory)
{
    clock_t startTime;

    cout << "Auto photo mode\n";
    for(int i = 0; i < Config1.Iterations; i++)
    {
        takePhoto(numCams, Config1.Exposure, CamInfo, directory);
        cout << "Waiting until next capture\n";
        startTime = clock();
        if (i == Config1.Iterations-1)
            while(Config1.Interval-Config1.Exposure/1000-1>= (clock()-startTime)/CLOCKS_PER_SEC){}
    }
}

void help ()
{
    cout << "Video Fireball v0.1\n\n";
    cout << "To store to local directory:\t./videofireball\n";
    cout << "To store to an address:\t\t./videofireball <address>\n";
    cout << "For auto mode:\t\t\t./videofireball auto\n";
    cout << "\t\t\t\t./videofireball <address> auto\n\n";
}

void gainTest(int numCams, Config Config1, ASI_CAMERA_INFO CamInfo[6], char* directory)
{
    int gain = 0;
    int camID = 0;
    char gainText[10];
    clock_t startTime;


    for(gain = 0; gain <= 500; gain+=25)
    {
        Config1.Gain = gain;
        for(camID = 0; camID < numCams; camID++)
            ASISetControlValue(camID,ASI_GAIN,Config1.Gain, ASI_FALSE);

        takePhoto(numCams, Config1.Exposure, CamInfo, directory);

        startTime = clock();
        while(1>= (clock()-startTime)/CLOCKS_PER_SEC){}
    }
}


void eventDetect(IplImage* capture[6], int numCams, int exposure, ASI_CAMERA_INFO CamInfo[6], int recTime, char* directory)
{
    char keepVid = 0;
    char cameraName[8];
    int camID = 0;
    IplImage * tempFrame = NULL;
    long frameCount = 0;
    IplImage* dst = NULL;
    int difference = 0;
    int aveDifference = 0;
    double var = 0;
    int duration = 5*60;
    clock_t startTime;
    int threshold = 5;

    startTime = clock();
    while(duration >= (clock()-startTime)/CLOCKS_PER_SEC)
    {
        for(camID = 0; camID < numCams; camID++)
            ASIStartVideoCapture(camID);

        if(capture[0])
            tempFrame = cvCloneImage(capture[0]);

        for(camID = 0; camID < numCams; camID++)
        {
            ASIGetVideoData(camID,(unsigned char*)capture[camID]->imageData,capture[camID]->imageSize,exposure);
        }

        if(tempFrame != NULL)
        {
            dst = cvCreateImage(cvSize(3096,2080), 8, 1);

            cvAbsDiff(tempFrame, capture[0], dst);
            difference = cvCountNonZero(dst);
            if (aveDifference == 0)
                aveDifference = difference;

            if(difference)
            {
                aveDifference = (int)((aveDifference+difference)/2);
                cout<< "Difference: " << difference << endl;
                cout<< "AverageVal: " << aveDifference << endl;
                if(aveDifference)
                {
                    var = 100*abs(1-(double)difference/(double)aveDifference);
                    printf("PercentVar=%lf\n",var);
                }

                if (var > threshold)
                {
                    cout << "Event Detected\n";
                    recordVideo(capture, numCams, exposure, CamInfo, recTime, directory);
                    aveDifference = 0;
                    var = 0;
                }
            }
        }

        cvReleaseImage(&tempFrame);
        cvReleaseImage(&dst);
    }
    cvReleaseImage(&tempFrame);    //close all and free memory

    for(camID = 0; camID < numCams; camID++)
        ASIStopVideoCapture(camID);
}
