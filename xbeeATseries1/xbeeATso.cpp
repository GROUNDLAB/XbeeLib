/*
 *  xbeeATso.cpp
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

#include "xbeeATso.h"

//*********************************************************************************
//constructor
xbeeATso::xbeeATso(){
for (int i=0;i<2; i++){ATcommand[i]='*';} // 2 AT max
}
//*********************************************************************************



//*********************************************************************************
boolean xbeeATso::sendLocalAtCommandWithOptions (char* _ATcommand, char* _AToptions,int _optionsize) {

  // set frame ID for call back
  static byte frameID = 0xA1;
  frameID++;                                // add one to the frame ID each time, okay to overflow
  if (frameID == 0) frameID = 1;            //skip zero because this disables status response packets

  int CommandLength = (strlen(_ATcommand)); // length of ATcommand
  int AToptionLength =  _optionsize;        // if there are options count them
  CommandLength += AToptionLength;

  //******************************
  //calculate length of the packet
  //Length = all bytes in *DATA FRAME* (no start byte, length byte or checksum)
  //(API identifier + frameId) = 2 "plus +" (ATcommand + command options) = CommandLength
  int messageLength = (CommandLength + 2);

  //A checksum is calculated and verified on non-escaped data.
  //To calculate: Not including frame delimiters and length,
  //add all bytes keeping only the lowest 8
  //bits of the result and subtract the result from 0xFF.
  //To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
  //correct, the sum will equal 0xFF.
  byte checksum = 0xFF;                 // checksums are the hex FF minus the total of all bytes
  //******************************
  // send the packet
  Serial.print(0x7E, BYTE);             // send start delimiter
  Serial.print(0x0, BYTE);              // send length MSB (always zero because packets can't be more than 100 bytes)
  Serial.print(messageLength, BYTE);    // send length LSB
  Serial.print(0x08, BYTE);             // send API command identifier **AT REMOTESEND= 0x17**
  checksum = checksum - 0x08;           // minus command identifier from checksum FF
  Serial.print(frameID, BYTE);          // send frame ID (set to 0 if noresponse is required)
  checksum = checksum - frameID;

  //******************************
  //command options
  Serial.print(_ATcommand[0], BYTE);    // send ATcommand
  checksum = checksum - _ATcommand[0];
  Serial.print(_ATcommand[1], BYTE);
  checksum = checksum - _ATcommand[1];
  //******************************
  //send options
  if(AToptionLength != 0){
    for (int i = 0; i < AToptionLength; i++){
      Serial.print(_AToptions[i] ,BYTE);
      checksum = checksum - _AToptions[i];
    }
  }

  Serial.print(checksum,BYTE);          // send checksum
  return true;
}
//*********************************************************************************



//*********************************************************************************
boolean xbeeATso::sendLocalAtCommand(char* _ATcommand) {
  // set frame ID for call back
  static byte frameID = 0xA1;
  frameID++;                                    // add one to the frame ID each time, okay to overflow
  if (frameID == 0) frameID = 1;                //skip zero because this disables status response packets

  int CommandLength = (strlen(_ATcommand));     // length of ATcommand

  //******************************
  //calculate length of the packet
  //Length = all bytes in *DATA FRAME* (no start byte, length byte or checksum)
  //(API identifier + frameId) = 2 "plus +" (ATcommand) = CommandLength
  int messageLength = (CommandLength + 2);

  //A checksum is calculated and verified on non-escaped data.
  //To calculate: Not including frame delimiters and length,
  //add all bytes keeping only the lowest 8
  //bits of the result and subtract the result from 0xFF.
  //To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
  //correct, the sum will equal 0xFF.
  byte checksum = 0xFF; // checksums are the hex FF minus the total of all bytes
  //******************************
  // send the packet
  Serial.print(0x7E, BYTE);             // send start delimiter
  Serial.print(0x0, BYTE);              // send length MSB (always zero because packets can't be more than 100 bytes)
  Serial.print(messageLength, BYTE);    // send length LSB
  Serial.print(0x08, BYTE);             // send API command identifier **AT REMOTESEND= 0x17**
  checksum = checksum - 0x08;           // minus command identifier from checksum FF
  Serial.print(frameID, BYTE);          // send frame ID (set to 0 if noresponse is required)
  checksum = checksum - frameID;

  //******************************
  //AT command
  Serial.print(_ATcommand[0], BYTE);    // send ATcommand
  checksum = checksum - _ATcommand[0];
  Serial.print(_ATcommand[1], BYTE);
  checksum = checksum - _ATcommand[1];
  Serial.print(checksum,BYTE);          // send checksum
  return true;
}
//*********************************************************************************




//*********************************************************************************


boolean xbeeATso::getLocalATCommandData(int _timeOut){

  unsigned int timeout = _timeOut;      // bail out time
  long startTime;

  byte inByte=0x00;
  startTime = millis();
  //***************
 //LOOKING FOR START BYTE AND PACKET LENGTH
while (inByte != 0x7E) {                           // clears out the buffer of junk
  if(Serial.available() < 1) return 0;             // no more and didn't find it
  inByte = Serial.read();                          // read a byte from the buffer
  if((millis() - startTime) > timeout)return 0;    // timed out bad message
}

  //***************
  //look for start byte 0x7E(~)
  if (inByte == 0x7E) {                             // if we got the API start byte
    startTime = millis();
    while (Serial.available() < 2){                 // block for length byte two bytes to be available
      if((millis() - startTime) > timeout)return 0; // timed out bad message
	}
    if (Serial.available() > 1){                    // if we got 2 bytes
      int lengthMSB = Serial.read();                // read the most significant length byte
      int lengthLSB = Serial.read();                // read the least significant length byte
      packetLength = (lengthMSB << 8) + lengthLSB;  // put the two bytes together
    }
  }

  //***************
  //  APIDENTIFIER
  if (packetLength > 0) {                           //IF WE HAVE A FULL PACKET
    apiIdentifier = 'Z';                            // set apiIdentifier to an impossible value
    startTime = millis();
    while (Serial.available() < 1){                 // wait for a byte or timeout
      if((millis() - startTime) > timeout) return 0;// timed out bad message
    }

    if (Serial.available() > 0) {                   // if a byte is waiting in the buffer set it to the API
      apiIdentifier = byte(Serial.read());
    }
    else return 0; //else bad message


    //***************
    //FRAME ID
    startTime = millis();
    while (Serial.available() < 1){                 // wait for a byte or timeout
      if((millis() - startTime) > timeout)return 0; // timed out bad message
    }
    if (Serial.available() > 0) {                   // if a byte is waiting in the buffer set it to the API
      frameID = byte(Serial.read());
    }
    else return 0; //else bad message


    //***************
    //GET AT COMMAND BYTE
    startTime = millis();
    char ATcommandByte = '?';                       // what is it?
    while( Serial.available() < 3 ){                // stop for bits
      if((millis() - startTime) > timeout) return 0;// timed out bad message
    }
    if (Serial.available() > 1) {                   // if a byte is waiting in the buffer
      for (int i = 0; i<2; i++){                    // two command bytes
        ATcommandByte = byte(Serial.read());
        ATcommand[i] = ATcommandByte;
      }
    }
    else return 0;                                  // timed out bad message


    //***************
    //GET STATUS/options
    startTime = millis();
    while (Serial.available() < 1){
      if((millis() - startTime) > timeout) return 0;// timed out bad message

    }
    status = byte(Serial.read());                   // get the options/status 0 = OK 1 = Error 2 = Invalid Command 3 = Invalid Parameter


    //***************
    //GET DATA
    startTime = millis();
    dataLength = (packetLength - 5);				        // record the lenth of incoming data
    while (Serial.available() < (packetLength - 5)){
      if((millis() - startTime) > timeout) return 0;
    }
    static boolean goneOnce = false;						// flag to not free the first time
    if (goneOnce)free(receivedData);						// if it has been allocated Free the memory
    else goneOnce = true;
    receivedData = (byte*) malloc(packetLength - 5);        // get memory
    if (receivedData == NULL){                              // if we couldn't get memory

      return 0;                                             // couldnt allocate
    }

    for (int i= 0; i < (packetLength - 5); i++) {			// take out other info in packet + checksu

      receivedData [i] = byte(Serial.read());				// get a byte of data
    }

    //***************
    //GET CHECKSUM
    startTime = millis();
    while (Serial.available() < 1){
      if((millis() - startTime) > timeout) return 0;        // timed out bad message
    }
    checksum = byte(Serial.read());


    //***************
    //PRINTALL
#ifdef XBEEDEBUG
    Serial.println("packetLength");
    Serial.println(packetLength,HEX);
    Serial.println("apiIdentifier");
    Serial.println(apiIdentifier,HEX);
    Serial.println("frameID");
    Serial.println(frameID,HEX);
    Serial.println("Atcommand");
    for (int i= 0; i < 2; i++) {                            // take out other info in packet + checksum
      Serial.println(ATcommand[i],HEX);
    }
    Serial.println("status");
    Serial.println(status,HEX);
    Serial.println("info");
    for (int i= 0; i < (packetLength - 5); i++) {           // take out other info in packet + checksum
      Serial.println(receivedData[i],HEX);
    }
    Serial.println("checksum  ");
    Serial.println(checksum,HEX);
    Serial.println("END");
#endif

    return 1;                                               // if everything went well return a true
  }
  return 0;                                                 // somthing went wrong
}


//*********************************************************************************
