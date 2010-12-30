/*
 *  xbeeATst.h
 *  xbeeLibrary

  Copyright (c) Justin Downs 12/9/08 All right reserved.
  for more info/tutorials visit
  www.johnHenrysHammer.com/WOW/
  or
  www.GRNDLAB.com

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */

#ifndef _XBEEATST
#define _XBEEATST
//#include <avr/interrupt.h>
//#include "WConstants.h"
#include <WProgram.h>
#include <inttypes.h>
class xbeeATst {
	public:
	xbeeATst();

	boolean sendAtCommand(long* _destination, char* _ATcommand, int _localAdress=0xFFFE );
	boolean sendAtCommandWithOptions (long* _destination, char* _ATcommand, char* _AToptions,int _optionsize, int _localAdress=0xFFFE );
	boolean getATCommandData(int _timeOut=500);

	boolean sendLocalAtCommandWithOptions (char* _ATcommand, char* _AToptions,int _optionsize);
	boolean sendLocalAtCommand(char* _ATcommand);
	boolean getLocalATCommandData(int _timeOut= 500);

	//value Return functions
	int getPacketLength() const {return packetLength;}
	byte getApiIdentifier() const {return apiIdentifier;}
	long getSendersHighAddress() const {return highAddress;}
	long getSendersLowAddress() const {return lowAddress;}
	int getSenderslocalAdress() const {return localAddress;}
	byte getOptions() const {return options;}
	byte* getAutoReceivedData() const {return receivedData;}
	int getDataLength() const { return dataLength;}
	byte getchecksum() const {return checksum;}
	//AT SPECIFIC RETURNS
	byte getframeID() const {return frameID;}
	const char* getATcommand() const {return ATcommand;}
	byte getstatus() const {return status;}

	private:
	int packetLength;   // length of the packet
	byte apiIdentifier; // the api identifier
	long highAddress;   // sentfrom high address
	long lowAddress;    // sentfrom low address
	int localAddress;   // local address it was sent from
	byte options;       // get the options: 1 means packet was acknowledged, returned 2 brodcast
	byte* receivedData; // the data in
	byte checksum;		// the checksum
	int dataLength;     // the length of incoming data
	//AT specific
	byte frameID;       // AT responce gives you frameID
	char ATcommand[2];  // AT responce what AT
    byte status;        // AT responce status 0 = OK 1 = Error 2 = Invalid Command 3 = Invalid Parameter

};

#endif

