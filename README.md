# Video Fireball

Software for capturing video from the ZWO ASI178MM (and other ZWO model cameras) for the purpose of fireball detection.

Based on the ASI SDF produced by ZWO, utilising OpenCV3.3 libraries.

Compatible only with linux, currently tested with:
	Ubuntu 16.04 LTS 64-bit,
	Debian 9.4.0 64-bit
	
Note that installation of INDI (Instrument Neutral Distributed Interface) drivers is not required for this camera, although it can be used with INDI drivers if preferred. 

## Installation Guide

### Step 1: Install OpenCV
One of the easiest ways to do this is using milq's script: https://github.com/milq/milq/blob/master/scripts/bash/install-opencv.sh

### Step 2: Install pcap and boost:
Using the following commands, install pcap and boost.
	$ apt-get install libpcap-dev
	$ apt-get install libboost-all-dev

### Step 3: Install device manager rules for the ASI camera
Retrieve the videoFireball program by copying from the complete folder “videoFireball” from this repository (https://github.com/BuiltByStu/videoFireball) 
The device manager udev must be given rules for use of the ZWO camera. To achieve this, in the location where the videoFireball folder has been copied or cloned, navigate to the folder videoFireball\lib. Run the following commands with the camera connected to the system using a USB3.0 port:

	$ install asi.rules /lib/udev/rules.d
	$ cat /sys/module/usbcore/parameters/usbfs_memory_mb

If this is successful, 200 will be returned, indicating that rules for the camera have successfully been established.

### Step 4: Make the video fireball program
Navigate into the videoFireball/src folder and run the following commands to link to the libraries. Replace x64 with x86 if the install is on a 32-bit system.
	
	$ LD_LIBRARY_PATH=$LD_LIBRARY_PATH../lib/x64
	$ export LD_LIBRARY_PATH

Using the command “make” compiles the code and the program is now ready to use.
