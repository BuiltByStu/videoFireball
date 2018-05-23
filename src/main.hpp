/*
File:		main.hpp
Authour:	Stuart Hahn
Version:	0.1
Date: 		23/05/18
Description:Header file for the videoFireball program.

Includes the following classes:
	Config

Includes the following functions:
    cameraDetect
    readConfiguration
    cameraCheck
    printCameraProperties
    printConfig
    previewVideo
    modeSelectMenu
    takePhoto
    recordVideo
    recordDuration
    recordDuration
    timeStamp
    timedCapture
    autoVideo
    autoPhoto
    gainTest
    eventDetect
    help
*/

//Class declarations

/*CLASS: Config
DESCRIPTION:Stores the variables which can be automatically configured from
            the config file*/
class Config {
    public:
        int Exposure;
		int Gain;
		int Gamma;
		int Interval;
        int Iterations;
        int VideoDuration;
};

//Function declarations

/*FUNCTION: cameraDetect
DESCIPTION: Tests, print and returns the number of ZWO cameras attached by USB
            Returns the number of cameras*/
int cameraDetect();


/*FUNCTION: readConfiguration
DESCIPTION: Read the config file and store the data
            Config1 is of class Config
            numCams is the number of attached ZWO cameras
            Returns 1 if successful, 0 if not*/
int readConfiguration(Config& Config1, int numCams);

/*FUNCTION: cameraCheck
DESCIPTION: Checks that the cameras can be started and intialised
            CamInfo is a struct containing information about the camera
            cameraID is the identification number of the camera (eg 0-5)
            Returns 1 if successful, 0 if not*/
int cameraCheck (ASI_CAMERA_INFO CamInfo, int cameraID);

/*FUNCTION: printCameraProperties
DESCIPTION: Print details about a connected camera
            CamInfo is a struct containing information about the camera
            cameraID is the identification number of the camera (eg 0-5)
            Returns 1 if successful, 0 if not*/
void printCameraProperties(ASI_CAMERA_INFO CamInfo, int cameraID);

/*FUNCTION: printConfig
DESCIPTION: Prints a config class to console to show values in UI
            Config1 is the Config class to be printed*/
void printConfig(Config& Config1);

/*FUNCTION: previewVideo
DESCIPTION: Preview video feed from cameras on display if attached
            Converts to 8 bit for preview for 16-bit inputs
            capture is an array of IPlImage pointers for storing latest image
            from each camera
            numCams is the number of cameras attached
            exposure is the exposure setting for the camera*/
void previewVideo(IplImage* capture[6], int numCams, int exposure);

/*FUNCTION: modeSelectMenu
DESCIPTION: Text based user interface for selecting program mode
            capture is an array of IPlImage pointers for storing latest image
            from each camera
            numCams is the number of cameras attached
            Config1 is the Config class containing data from the config file
            CamInfo is an array of struct containing information about each
            camera
            directory is the location to store images and videos*/
void modeSelectMenu(IplImage* capture[6], int numCams, Config Config1,
ASI_CAMERA_INFO CamInfo[6], char* directory);

/*FUNCTION: takePhoto
DESCIPTION:Tales a still photo from the instructed number of cameras at a
            given esposure and stores it to the instructed directory
            numCams is the number of cameras attached
            exposure is the exposure setting for the camera
            CamInfo is an array of struct containing information about each
            camera
            directory is the location to store images*/
void takePhoto(int numCams, int exposure, ASI_CAMERA_INFO CamInfo[6],
char* directory);

/*FUNCTION: recordVideo
DESCIPTION: Record a video of a given duration to a given location using
            given exposure. All other settings from config file
            capture is an array of IPlImage pointers for storing latest image
            from each camera
            numCams is the number of cameras attached
            exposure is the exposure setting for the camera
            CamInfo is an array of struct containing information about each
            camera
            recTime is the video recording duration
            directory is the location to store images*/
void recordVideo(IplImage* capture[6], int numCams, int exposure,
ASI_CAMERA_INFO CamInfo[6], int recTime, char* directory);

/*FUNCTION: recordDuration
DESCIPTION: Prompts user for the duration of video to record and calls
            recordVideo to do recording of this length
            capture is an array of IPlImage pointers for storing latest image
            from each camera
            numCams is the number of cameras attached
            exposure is the exposure setting for the camera
            CamInfo is an array of struct containing information about each
            camera
            directory is the location to store images*/
void recordDuration(IplImage* capture[6], int numCams, int exposure,
ASI_CAMERA_INFO CamInfo[6], char* directory);

/*FUNCTION: timeStamp
DESCIPTION: Returns a string containing the current date and time in the
            format YYYYMMDDHHmmSS when called*/
std::string timeStamp();

/*FUNCTION: timedCapture
DESCIPTION: Queues the caputre of videos or images based on inputs taken from
            user for type, duration, frequency and record duration(for video)
            capture is an array of IPlImage pointers for storing latest image
            from each camera
            numCams is the number of cameras attached
            Config1 is the Config class containing data from the config file
            CamInfo is an array of struct containing information about each
            camera
            directory is the location to store images or videos*/
void timedCapture(IplImage* capture[6], int numCams, Config Config1,
ASI_CAMERA_INFO CamInfo[6], char* directory);

/*FUNCTION: autoVideo
DESCIPTION: Autnomously captures videos based on the settings provided
            capture is an array of IPlImage pointers for storing latest image
            from each camera
            numCams is the number of cameras attached
            Config1 is the Config class containing data from the config file
            CamInfo is an array of struct containing information about each
            camera
            directory is the location to store videos*/
void autoVideo(IplImage* capture[6], int numCams, Config Config1,
ASI_CAMERA_INFO CamInfo[6], char* directory);

/*FUNCTION: autoPhoto
DESCIPTION: Autnomously captures photos based on the settings provided
            numCams is the number of cameras attached
            Config1 is the Config class containing data from the config file
            CamInfo is an array of struct containing information about each
            camera
            directory is the location to store images*/
//Autonomous photo mode
void autoPhoto(int numCams, Config Config1, ASI_CAMERA_INFO CamInfo[6],
char* directory);

/*FUNCTION: gainTest
DESCIPTION: Takes photos at settings read from config file, other than gain.
            Takes photos sequentially starting at gain = 0 through to gain =
            500 at incrememnts of 25 with 1 second pause between images
            numCams is the number of cameras attached
            Config1 is the Config class containing data from the config file
            CamInfo is an array of struct containing information about each
            camera
            directory is the location to store images*/
void gainTest(int numCams, Config Config1, ASI_CAMERA_INFO CamInfo[6],
char* directory);

/*FUNCTION: eventDetect
DESCIPTION: Tests the absolute difference between two sequential frames from
            camera and runs average of difference, if difference exceeds
            percentage variaton from average threshold, video recording is
            started. Only functioning for single camera in current build
            capture is an array of IPlImage pointers for storing latest image
            from each camera
            numCams is the number of cameras attached
            exposure is the exposure setting for the camera
            CamInfo is an array of struct containing information about each
            camera
            directory is the location to store images*/
//detect movemetn
void eventDetect(IplImage* capture[6], int numCams, int exposure,
ASI_CAMERA_INFO CamInfo[6], int recTime, char* directory);

/*FUNCTION: help
DESCIPTION: Provides instructions to user of how to use the basic functions
            of the program through print to console. Can be called at program
            start using: ./videoFireball help*/
void help();

