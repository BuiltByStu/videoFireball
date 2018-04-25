/*
File:		main.cpp
Authour:	Stuart Hahn
Version:	0.1
Date: 		30/03/18
Description:Header file for main.cpp.

Includes the following classes:
	Config

Includes the following functions:
	 printConfig

*/

//Class declarations
class Config {
    public:
        int Exposure;
		int Gain;
		int Gamma;
		int Interval;
        int Iterations;
        int VideoDuration;
};

//function declarations

//Test the number of cameras, prints and returns the number
int cameraDetect();

//Read the config file and store the configuration
//Config1 is the class which stores the configuration data
int readConfiguration(Config& Config1);

//Checks that the cameras can be started and intialised
//CamInfo is a struct containing information about the camera
//CameraID is the identification number of the camera (0-5)
int cameraCheck (ASI_CAMERA_INFO CamInfo, int cameraID);

//Print details about a connected camera
//CamInfo is a struct containing information about the camera
//CameraID is the identification number of the camera (0-5)
void printCameraProperties(ASI_CAMERA_INFO CamInfo, int cameraID);

//For checking on the values stored to config class
void printConfig(Config& Config1);

//Preview video feed from cameras-converts to 8 bit for preview
void previewVideo(IplImage* capture[6], int numCams, int exposure);

//menu for selecting program mode
void modeSelectMenu (IplImage* capture[6], int numCams, Config Config1, ASI_CAMERA_INFO CamInfo[6], char* directory);

//Take a still photograph
void takePhoto(IplImage* capture[6], int numCams, int exposure, ASI_CAMERA_INFO CamInfo[6], char* directory);

//Record video
void recordVideo(IplImage* capture[6], int numCams, int exposure, ASI_CAMERA_INFO CamInfo[6], int recTime, char* directory);

//Records video of input duration
void recordDuration(IplImage* capture[6], int numCams, int exposure, ASI_CAMERA_INFO CamInfo[6], char* directory);

//Returns the current date and time in the form year-month-day-hour-min-sec
std::string timeStamp();

void timedCapture(IplImage* capture[6], int numCams, Config Config1, ASI_CAMERA_INFO CamInfo[6], char* directory);

//Autonomous video mode
void autoVideo(IplImage* capture[6], int numCams, Config Config1, ASI_CAMERA_INFO CamInfo[6], char* directory);

//Autonomous photo mode
void autoPhoto(IplImage* capture[6], int numCams, Config Config1, ASI_CAMERA_INFO CamInfo[6], char* directory);

//Lens and camera calibration mode
//void calibration();
