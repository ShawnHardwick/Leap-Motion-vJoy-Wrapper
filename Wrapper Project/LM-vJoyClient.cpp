/******************************************************************************\
*Red Panda, INC.															   *
*Founding Members: Shawn Hardwick, Richard Jones                               *
*Project Title: LeapMotion vJoy Wrapper                                        *
*CSCI 490                                                                      *
*Professor: Dr. French                                                         *
\******************************************************************************/


/******************************************************************************\
* Copyright (C) 2012-2013 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/


/******************************************************************************\
*Copyright (C) 2010-2012 vJoystick. Open source device driver.				   *
*http://vjoystick.sourceforge.net/site/										   *
*vJoystick is an open source device driver that will allow the user to use a   *
*device that is not support by an application. The feeder sample was made and  * 
*compiled by the maker(s) of vJoystick. They can be contacted at               *
* http://vjoystick.sourceforge.net/site/index.php/contact-form1.               *
*All rights remane to vJoystick; if the have any.                              *
\******************************************************************************/

#include <iostream>
#include "stdafx.h"
#include "public.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "LeapListener.h"
#include "vJoyInterface.h"

using namespace Leap;

int
__cdecl
_tmain(__in int argc, __in PZPWSTR argv){
  // Create a sample listener and controller
  SampleListener listener;
  Controller controller;

  // Have the sample listener receive events from the controller
  controller.addListener(listener);
  controller.setPolicyFlags(Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES); //Allows background frame tracking when application is out of focus

	USHORT X;										//Position of Axis
	JOYSTICK_POSITION	iReport;					// The structure that holds the full position data
	BYTE id=1;										// ID of the target vjoy device (Default is 1)
	UINT iInterface=1;								// Default target vJoy device
	BOOL ContinuousPOV=FALSE;						// Continuous POV hat (or 4-direction POV Hat)
	int count=0;


	// Get the ID of the target vJoy device
	if (argc>1 && wcslen(argv[1]))
		sscanf_s((char *)(argv[1]), "%d", &iInterface);


	// Get the driver attributes (Vendor ID, Product ID, Version Number)
	if (!vJoyEnabled())
	{
		_tprintf("vJoy driver not enabled: Failed Getting vJoy attributes.\n");
		return -2;
	}

	// Get the state of the requested device
	VjdStat status = GetVJDStatus(iInterface);
	switch (status)
	{
	case VJD_STAT_OWN:
		_tprintf("vJoy Device %d is already owned by this feeder\n", iInterface);
		break;
	case VJD_STAT_FREE:
		_tprintf("vJoy Device %d is free\n", iInterface);
		break;
	case VJD_STAT_BUSY:
		_tprintf("vJoy Device %d is already owned by another feeder\nCannot continue\n", iInterface);
		return -3;
	case VJD_STAT_MISS:
		_tprintf("vJoy Device %d is not installed or disabled\nCannot continue\n", iInterface);
		return -4;
	default:
		_tprintf("vJoy Device %d general error\nCannot continue\n", iInterface);
		return -1;
	};


	// Acquire the target
	if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(iInterface))))
	{
		_tprintf("Failed to acquire vJoy device number %d.\n", iInterface);
		return -1;
	}
	else
	{
		_tprintf("Acquired: vJoy device number %d.\n", iInterface);
	}

	long value = 0;
	BOOL res = FALSE;

  //// Keep this process running until Enter is pressed
  //std::cout << "Press Enter to quit..." << std::endl;
  //std::cin.get();
	X = 0;
  // Reset this device to default values
	ResetVJD(iInterface);
	while(1)
	{
		//create the data packet that holds all the information
		id = (BYTE)iInterface;
		iReport.bDevice = id;
		iReport.wAxisX=X;

		const Frame frame = controller.frame();
		const Frame prevFrame = controller.frame(1);
		if (!frame.hands().isEmpty()) {
			const Hand hand = frame.hands()[0];
			const FingerList fingers = hand.fingers();
			if (!fingers.isEmpty()) {

				//SetBtn(TRUE, iInterface, fingers.count());
			}
			float rotation = hand.palmNormal().roll();
	
			int axisValue;
			if(rotation > 0){
				axisValue = 35900 - (rotation * 19516 + 16384);
			}
			else if (rotation < 0){
				axisValue = (abs(rotation) * 19516 + 16384);
			}
			else{ //Hand is center
				axisValue = 16384;
			}
			
			// Sets the X Axis
			//SetAxis(axisValue, iInterface, HID_USAGE_X);
			iReport.wAxisX = axisValue;

			//Count the amount of fingers in FingerList to set button
			//2 or more fingers, Accelerate
			int howmanyfingers = fingers.count();

			if(howmanyfingers >= 2) {
				//SetBtn(TRUE, iInterface, 1);
				//SetBtn(FALSE, iInterface, 2);
				iReport.lButtons = 1;
			}
			//1 or less fingers, Decelerate
			else if (howmanyfingers <= 1) {
				//SetBtn(TRUE, iInterface, 2);
				//SetBtn(FALSE, iInterface, 1);
				iReport.lButtons = 2;
			}
			
		}
		else{
			//SetAxis(16384, iInterface, HID_USAGE_X); //Set axis to neutral if no hand is detected
			//SetBtn(FALSE, iInterface, 1); //Set buttons to false if no hand is detected
			//SetBtn(FALSE, iInterface, 2); //Set buttons to false if no hand is detected
			iReport.lButtons = 0;
			iReport.wAxisX = 16384;
		} 

		UpdateVJD(iInterface, (PVOID)&iReport); //updates with iReport

	}

  // Remove the sample listener when done
  RelinquishVJD(iInterface);
  controller.removeListener(listener);

  return 0;
}
