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
		int WB_R;
		int WB_B;
		int Bandwidth;
		int HS_Mode;
};

//function declarations

//Test the number of cameras, prints and returns the number
int cameraDetect();

//Read the config file and store the configuration
int readConfiguration(Config& Config1);

//For checking on the values stored to config class
void printConfig(Config& Config1);


