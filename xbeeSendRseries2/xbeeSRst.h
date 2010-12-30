/*
 *  xbeeSRst.h
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


#ifndef _XBEESRST
#define _XBEESRST

//ARDUINO INCLUDES FOR SERIAL
//#include <avr/interrupt.h>  //V pre 14
//#include "WConstants.h"     //V 14
#include <WProgram.h>         //V 16
#include <inttypes.h>         //V 16
class xbeeSRst {

	public:
	xbeeSRst();
	boolean sendData(byte* data, int _dataLength,long* _destination,int _localAdress=0xFFFE);
	boolean getDataAuto(int _timeOut=500);                   //automaticaly allocates memory for incoming data
	boolean getData(byte * _receivedData, int _timeOut=500); //declares what array to use for incoming data

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

};



#endif

