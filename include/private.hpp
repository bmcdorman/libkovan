/**************************************************************************
 *  Copyright 2012 KISS Institute for Practical Robotics                  *
 *                                                                        *
 *  This file is part of KISS (Kipr's Instructional Software System).     *
 *                                                                        *
 *  KISS is free software: you can redistribute it and/or modify          *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  KISS is distributed in the hope that it will be useful,               *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with KISS.  Check the LICENSE file in the project root.         *
 *  If not, see <http://www.gnu.org/licenses/>.                           *
 **************************************************************************/

#ifndef _PRIVATE_HPP_
#define _PRIVATE_HPP_

#include <pthread.h>
#include <sys/time.h>

#define SHARED_MEMORY_KEY 8374

#define NUM_DIGITALS 8
#define NUM_ANALOGS 8
#define NUM_MOTORS 4
#define NUM_SERVOS 4
#define MAX_BUTTON_TEXT_SIZE 16

namespace Private
{
	struct PID {
		short p;
		short i;
		short d;
		short pd;
		short id;
		short dd;
	};
	
	enum MotorControlMode {
		PIDMode = 0,
		PWMMode
	};
	
	namespace MotorDirection
	{
		enum MotorDirection {
			PassiveStop = 0,
			Reverse,
			Forward,
			ActiveStop
		};
	}
	
	// Server writes this data. Clients read.
	struct SharedMemoryServer
	{
		bool testFlag;
		
		timeval timestamp;
		unsigned long updates;
		unsigned short servoPositions[NUM_SERVOS];
		
		unsigned char digitals : NUM_DIGITALS;
		unsigned char digitalDirections : NUM_DIGITALS;
		unsigned char digitalPullups : NUM_DIGITALS;
		
		unsigned short analogs[NUM_ANALOGS];
		
		MotorControlMode motorControlMode;
		unsigned char pwms[NUM_MOTORS];
		PID pids[NUM_MOTORS];
		
		unsigned short backEMFs[NUM_MOTORS * 2];

		unsigned short rawBatteryVoltage;
	};
	
	// Clients write this data. Server reads.
	struct SharedMemoryClient
	{
		bool testFlag;
		
		// Motors
		
		bool motorControlModeDirty : 1;
		MotorControlMode motorControlMode;
		
		unsigned char pwmDirty : NUM_MOTORS; // Lower 4 bits used
		unsigned char pwms[NUM_MOTORS];
		
		bool motorDirectionsDirty : 1;
		unsigned char motorDirections;

		unsigned char pidDirty : NUM_MOTORS; // Lower 4 bits used
		PID pids[NUM_MOTORS];
		
		// Servos
		unsigned char servoDirty : NUM_SERVOS;
		unsigned short servoPositions[NUM_SERVOS];
		
		// Analog
		
		unsigned char analogPullupsDirty : NUM_ANALOGS;
		unsigned char analogPullups :NUM_ANALOGS;
		
		// Digital
		
		unsigned char digitalPullupsDirty : NUM_DIGITALS;
		unsigned char digitalPullups : NUM_DIGITALS;
		unsigned char digitalDirectionsDirty : NUM_DIGITALS;
		unsigned char digitalDirections : NUM_DIGITALS;
		unsigned char digitalsDirty : NUM_DIGITALS;
		unsigned char digitals : NUM_DIGITALS;
	};
	
	struct SharedButton
	{
		bool textDirty : 1;
		char text[MAX_BUTTON_TEXT_SIZE];
		bool pressed : 1;
	};
	
	// Clients read and write this data;
	struct SharedMemoryInterClient
	{
		SharedButton a;
		SharedButton b;
		SharedButton c;
		
		bool isShowExtraButtonsDirty : 1;
		bool showExtraButtons : 1;
		
		SharedButton x;
		SharedButton y;
		SharedButton z;
	};
	
	struct SharedMemory
	{
		pthread_mutex_t serverMutex;
		SharedMemoryServer server;
		
		pthread_mutex_t clientMutex;
		SharedMemoryClient client;
		
		pthread_mutex_t interClientMutex;
		SharedMemoryInterClient interClient;
	};
	
	bool initSharedMemory(Private::SharedMemory *shm);
}

#endif
