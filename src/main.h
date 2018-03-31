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
		int Bandwidth;
		int HS_Mode;
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


