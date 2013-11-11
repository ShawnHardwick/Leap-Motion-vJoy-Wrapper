/******************************************************************************\
* Copyright (C) 2012-2013 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>
#include "stdafx.h"
#include "public.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "LeapListener.h"
#include "vjoyinterface.h"
using namespace Leap;

int
__cdecl
_tmain(__in int argc, __in PZPWSTR argv){
  // Create a sample listener and controller
  SampleListener listener;
  Controller controller;

  // Have the sample listener receive events from the controller
  controller.addListener(listener);

  USHORT X, Y, Z, ZR, XR;							// Position of several axes
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
	else
	{
		_tprintf("Vendor: %S\nProduct :%S\nVersion Number:%S\n", TEXT(GetvJoyManufacturerString()),  TEXT(GetvJoyProductString()), TEXT(GetvJoySerialNumberString()));
	};

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

	// Check which axes are supported
	BOOL AxisX  = GetVJDAxisExist(iInterface, HID_USAGE_X);
	BOOL AxisY  = GetVJDAxisExist(iInterface, HID_USAGE_Y);
	BOOL AxisZ  = GetVJDAxisExist(iInterface, HID_USAGE_Z);
	BOOL AxisRX = GetVJDAxisExist(iInterface, HID_USAGE_RX);
	BOOL AxisRZ = GetVJDAxisExist(iInterface, HID_USAGE_RZ);
	// Get the number of buttons and POV Hat switchessupported by this vJoy device
	int nButtons  = GetVJDButtonNumber(iInterface);
	int ContPovNumber = GetVJDContPovNumber(iInterface);
	int DiscPovNumber = GetVJDDiscPovNumber(iInterface);

	// Print results
	_tprintf("\nvJoy Device %d capabilities:\n", iInterface);
	_tprintf("Numner of buttons\t\t%d\n", nButtons);
	_tprintf("Numner of Continuous POVs\t%d\n", ContPovNumber);
	_tprintf("Numner of Descrete POVs\t\t%d\n", DiscPovNumber);
	_tprintf("Axis X\t\t%s\n", AxisX?"Yes":"No");
	_tprintf("Axis Y\t\t%s\n", AxisX?"Yes":"No");
	_tprintf("Axis Z\t\t%s\n", AxisX?"Yes":"No");
	_tprintf("Axis Rx\t\t%s\n", AxisRX?"Yes":"No");
	_tprintf("Axis Rz\t\t%s\n", AxisRZ?"Yes":"No");



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



	_tprintf("\npress enter to stat feeding");
	getchar();

	X = 20;
	Y = 30;
	Z = 40;
	XR = 60;
	ZR = 80;

	long value = 0;
	BOOL res = FALSE;

  // Keep this process running until Enter is pressed
  std::cout << "Press Enter to quit..." << std::endl;
  std::cin.get();

  // Remove the sample listener when done
  controller.removeListener(listener);

  return 0;
}
