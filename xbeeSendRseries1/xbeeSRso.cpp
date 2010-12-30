/*
   xbeeSRso.cpp
   xbeeLibrary
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
#include "xbeeSRso.h"

//constructor
xbeeSRso::xbeeSRso(){
packetLength = 0;
highAddress = 0;
lowAddress = 0;

}


//gets data puts it in Malloc array
boolean xbeeSRso::getDataAuto(int _timeOut) {   // call when there is serial data
unsigned int timeout= _timeOut;                 // bail out time
long startTime;


byte inByte=0x00;
startTime = millis();
//***************
//***************
//LOOKING FOR START BYTE AND PACKET LENGTH
while (inByte != 0x7E) {                        // clears out the buffer of junk
  if(Serial.available() < 1) return 0;          // no more and didn't find it
  inByte = Serial.read();                       // read a byte from the buffer
  if((millis() - startTime) > timeout)return 0; // timed out bad message
}


//***************
//look for start byte 0x7E(~)
if (inByte == 0x7E) {                            // if we got the API start byte
startTime = millis();
while (Serial.available() < 2){                  // block for length byte two bytes to be available
  if((millis() - startTime) > timeout)return 0;  // timed out bad message
}

//***************
//LENGTH BYTES
if (Serial.available() > 1){                     // if we got 2 bytes
  int lengthMSB = Serial.read();                 // read the most significant length byte
  int lengthLSB = Serial.read();                 // read the least significant length byte
  packetLength = (lengthMSB << 8) + lengthLSB;   // put the two bytes together
}
}

//***************
// APIDENTIFIER
if (packetLength > 0) {
apiIdentifier = 'Z';                            // set apiIdentifier to an impossible value
startTime = millis();
while (Serial.available() < 1){                 // wait for a byte or timeout
  if((millis() - startTime) > timeout) return 0;// timed out bad message
}
if (Serial.available() > 0) {                   // if a byte is waiting in the buffer set it to the API
  apiIdentifier = byte(Serial.read());
} else return 0;                                //else bad message

if(apiIdentifier == 0x80){                      //if it is using long address
//***************
//GET LONG ADDRESS
long addrByte=0;
highAddress=0;                                  // clear out address
lowAddress=0;
startTime = millis();
while(Serial.available() < 8){                  // stop for bytes
  if((millis() - startTime) > timeout) return 0;// timed out bad message

}
if (Serial.available() > 7) {
  byte address[8];                              // array for address
  for (int i = 0; i< 8; i++){
	addrByte = byte(Serial.read());
	address[i] = addrByte;

	//global last send
	if (i < 4){                                 // get first half of address
	highAddress += (addrByte << ((3-i) * 8));   // get the first 4 bytes shift into place 4321
	}else {lowAddress += (addrByte << ((7-i) * 8)); // get second shift them int place 4321
		}
  }

}else return 0;                                 // timed out bad message

}else if(apiIdentifier == 0x81){                //check to see if it is using short address
//***************
//GET LOCAL ADDRESS
startTime = millis();
while (Serial.available() < 2){                 // wait for at least two bytes to be available

if((millis() - startTime) > timeout) return 0;  // timed out bad message
}
int localAd = Serial.read();                    // read the most significant length byte
int localAd2 = Serial.read();                   // read the least significant length byte
localAddress = (localAd << 8) + localAd2;       // put the two bytes together

}

//***************
//GET RSSI signalStrength
startTime = millis();
while (Serial.available() < 1){
if((millis() - startTime) > timeout) return 0;  // timed out bad message
}
  RSSI = byte(Serial.read());                   // get the options 1 packet ack returned 2 broadcast

 //***************
//GET OPTIONS
startTime = millis();
while (Serial.available() < 1){
if((millis() - startTime) > timeout) return 0;  // timed out bad message
}
  options = byte(Serial.read());                // get the options 1 address broadcast returned 2  PAN broadcast



//***************
//GET DATA
 startTime = millis();
 dataLength = (packetLength - 12);			    //record the length of incoming data
 while (Serial.available() < (packetLength - 12)){
 if((millis() - startTime) > timeout) return 0;
 }

 static boolean goneOnce = false;						// flag to not free the first time
 if (goneOnce)free(receivedData);						// if it has been allocated Free the memory
 else goneOnce = true;
  receivedData = (byte*) malloc(packetLength - 12);     // get memory,
  if (receivedData == NULL) return 0;	                // if we couldn't get memory

 for (int i= 0; i < (packetLength - 12); i++) {			// take out other info in packet + checksum
 receivedData [i] = byte(Serial.read());				// get a byte of data
 }


//***************
//GET CHECKSUM
startTime = millis();
while (Serial.available() < 1){
if((millis() - startTime) > timeout) return 0;          // timed out bad message
}
checksum = byte(Serial.read());
}

return 1;                                               // if everything went well return a true
}









// function that puts together a data packet and transmits it, using 64-bit addressing
boolean xbeeSRso::sendData64(byte* data, int _dataLength,long* _destination) {
int dataLength = _dataLength;

static byte frameID = 0xA1;
frameID++;                              // add one to the frame ID each time, okay to overflow
if (frameID == 0) frameID = 1;          // skip zero because this disables status response packets
byte checksum = 0xFF;                   // checksums are the hex FF minus the total of all bytes
// calculate length of the packet
// API identifier + frameId + 8bytes 64-bit addr
// + 2 bytes 16-bit addr + broadcast radius
// + options
int length = (dataLength + 14);         // last one is tagByte
// send the packet
Serial.print(0x7E, BYTE);               // send start delimiter
Serial.print(0x0, BYTE);                // send length MSB (always zero because packets can't be more than 100 bytes)
Serial.print(length, BYTE);             // send length LSB
Serial.print(0x00, BYTE);               // send API command identifier
checksum = checksum - 0x00;             // minus from checksum FF
Serial.print(frameID, BYTE);            // send frame ID (set to 0 if no response is required)
checksum = checksum - frameID;

// set  the 64 bit factory address long high byte then low
for(int i=0;i < 2; i++){                                // address never bigger than 2
for (int j = 4; j > 0; j--) {
  byte destinationByte = _destination[i] >> 8 * (j-1);  // shift over one byte at a time, MSB first 4 for long
  Serial.print(destinationByte, BYTE);                  // send destination address
  checksum = checksum - destinationByte;
}
}


Serial.print(0x0,BYTE);                 // send option 0x01 = Disable ACK 0x04 = Send packet with Broadcast Pan ID All other bits must be set to 0.
checksum = checksum - 0x0;
//////******start data *****\\\\\\\\\\

//send all data in array
byte thisbyte;
for (int i = 0; i < dataLength ; i++){
 thisbyte = data[i];
Serial.print(thisbyte,BYTE);
checksum = checksum - thisbyte;
}


Serial.print(checksum,BYTE);           // send checksum
return true;

}






//sfunction that puts together a data packet and transmits it, using 64-bit addressing
boolean xbeeSRso::sendData16(byte* data, int _dataLength,int _localAddress) {
int dataLength = _dataLength;
//int dataLength = sizeof(data);
//strlen dosen't work with num conversion because of null use only with letter strings
//dataLength =(strlen(data)); // length of data when using CHAR ARRAYS ONLY
static byte frameID = 0xA1;
frameID++;                              // add one to the frame ID each time, okay to overflow
if (frameID == 0) frameID = 1;          // skip zero because this disables status response packets
byte checksum = 0xFF;                   // checksums are the hex FF minus the total of all bytes
// calculate length of the packet
// API identifier + frameId + 8bytes 64-bit addr
// + 2 bytes 16-bit addr + broadcast radius
// + options + ***TAGBYTE***
int length = (dataLength + 14);         // last one is tagByte
// send the packet
Serial.print(0x7E, BYTE);               // send start delimiter
Serial.print(0x0, BYTE);                // send length MSB (always zero because packets can't be more than 100 bytes)
Serial.print(length, BYTE);             // send length LSB
Serial.print(0x00, BYTE);               // send API command identifier
checksum = checksum - 0x10;             // minus from checksum FF
Serial.print(frameID, BYTE);            // send frame ID (set to 0 if no response is required)
checksum = checksum - frameID;
//********************************
// break up local address and send it
byte localLow = _localAddress;
byte localHigh = _localAddress >> 8;
//send 16-bit address as unkown FUNCTION
Serial.print(localHigh,BYTE);           // send unknown for 16-bit address FF
checksum = checksum-localHigh;
Serial.print(localLow,BYTE);            // send unknown for 16-bit address  FE
checksum = checksum - localLow;
Serial.print(0x0,BYTE);                 // send option 0x01 = Disable ACK 0x04 = Send packet with Broadcast Pan ID All other bits must be set to 0.
checksum = checksum - 0x0;
//////******start data *****\\\\\\\\\\

//send all data in array
byte thisbyte;
for (int i = 0; i < dataLength ; i++){
 thisbyte = data[i];
Serial.print(thisbyte,BYTE);
checksum = checksum - thisbyte;
}


Serial.print(checksum,BYTE);            // send checksum
return true;

}







